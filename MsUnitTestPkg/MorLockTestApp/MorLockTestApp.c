/** @file -- MorLockTestApp.c
This application will test the MorLock v1 and v2 variable protection feature.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2016 Microsoft Corporation. All Rights Reserved.

**/

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UnitTestLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Guid/MemoryOverwriteControl.h>
#include <IndustryStandard/MemoryOverwriteRequestControlLock.h>


#define UNIT_TEST_APP_NAME        L"MORLock v1 and v2 Test"
#define UNIT_TEST_APP_SHORT_NAME  L"MorLock_v1_and_v2_Test"
#define UNIT_TEST_APP_VERSION     L"0.1"

#define MOR_LOCK_DATA_UNLOCKED           0x0
#define MOR_LOCK_DATA_LOCKED_WITHOUT_KEY 0x1
#define MOR_LOCK_DATA_LOCKED_WITH_KEY    0x2

#define MOR_LOCK_V1_SIZE      1
#define MOR_LOCK_V2_KEY_SIZE  8

#define MOR_VARIABLE_ATTRIBUTES       (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
#define MOR_VARIABLE_BAD_ATTRIBUTES1  (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS)
#define MOR_VARIABLE_BAD_ATTRIBUTES2  (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)

UINT8     mTestKey1[] = { 0xD5, 0x80, 0xC6, 0x1D, 0x84, 0x44, 0x4E, 0x87 };
UINT8     mTestKey2[] = { 0x94, 0x88, 0x8F, 0xFE, 0x1D, 0x6C, 0xE0, 0x68 };
UINT8     mTestKey3[] = { 0x81, 0x51, 0x1E, 0x00, 0xCB, 0xFE, 0x48, 0xD9 };


///================================================================================================
///================================================================================================
///
/// HELPER FUNCTIONS
///
///================================================================================================
///================================================================================================


UNIT_TEST_STATUS
EFIAPI
MorControlVariableShouldBeCorrect (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINT32        Attributes;
  UINTN         DataSize;
  UINT8         Data;

  UT_LOG_VERBOSE( __FUNCTION__"()\n" );

  DataSize = sizeof( Data );
  Status = gRT->GetVariable( MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                             &gEfiMemoryOverwriteControlDataGuid,
                             &Attributes,
                             &DataSize,
                             &Data );

  return (UT_ASSERT_NOT_EFI_ERROR( Status ) &&
          UT_ASSERT_EQUAL( Attributes, MOR_VARIABLE_ATTRIBUTES ) &&
          UT_ASSERT_EQUAL( DataSize, sizeof( Data ) )) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorControlVariableShouldBeCorrect()


STATIC
EFI_STATUS
GetMorControlVariable (
  OUT UINT8       *MorControl
  )
{
  EFI_STATUS    Status;
  UINTN         DataSize;
  UINT8         Data;

  DataSize = sizeof( Data );
  Status = gRT->GetVariable( MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                             &gEfiMemoryOverwriteControlDataGuid,
                             NULL,
                             &DataSize,
                             &Data );

  if (!EFI_ERROR( Status ))
  {
    if (DataSize != sizeof( *MorControl ))
    {
      Status = EFI_BAD_BUFFER_SIZE;
    }
    else
    {
      *MorControl = Data;
    }
  }

  return Status;
} // GetMorControlVariable()


STATIC
EFI_STATUS
SetMorControlVariable (
  IN UINT8       *MorControl
  )
{
  EFI_STATUS    Status;

  Status = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                             &gEfiMemoryOverwriteControlDataGuid,
                             MOR_VARIABLE_ATTRIBUTES,
                             sizeof( *MorControl ),
                             MorControl );

  return Status;
} // SetMorControlVariable()


STATIC
VOID
UnitTestCleanupReboot (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework
  )
{
  // Reboot this mother.
  SaveFrameworkStateAndReboot( Framework, NULL, 0, EfiResetCold );
  return;
} // UnitTestCleanupReboot()


STATIC
EFI_STATUS
GetMorLockVariable (
  OUT UINT8       *MorLock
  )
{
  EFI_STATUS    Status;
  UINTN         DataSize;
  UINT8         Data;

  DataSize = sizeof( Data );
  Status = gRT->GetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                             &gEfiMemoryOverwriteRequestControlLockGuid,
                             NULL,
                             &DataSize,
                             &Data );

  if (!EFI_ERROR( Status ))
  {
    if (DataSize != sizeof( *MorLock ))
    {
      Status = EFI_BAD_BUFFER_SIZE;
    }
    else
    {
      *MorLock = Data;
    }
  }

  return Status;
} // GetMorLockVariable()


UNIT_TEST_STATUS
EFIAPI
MorLockShouldNotBeSet (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINT8         MorLock;

  UT_LOG_VERBOSE( __FUNCTION__"()\n" );

  Status = GetMorLockVariable( &MorLock );

  return ((Status == EFI_NOT_FOUND) ||
          (UT_ASSERT_NOT_EFI_ERROR( Status ) && UT_ASSERT_EQUAL( MorLock, MOR_LOCK_DATA_UNLOCKED ))) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockShouldNotBeSet()


///================================================================================================
///================================================================================================
///
/// TEST CASES
///
///================================================================================================
///================================================================================================


UNIT_TEST_STATUS
EFIAPI
MorControlVariableShouldExist (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINTN         DataSize;
  UINT8         Data;

  DataSize = sizeof( Data );
  Status = gRT->GetVariable( MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                             &gEfiMemoryOverwriteControlDataGuid,
                             NULL,
                             &DataSize,
                             &Data );

  return UT_ASSERT_NOT_EQUAL( Status, EFI_NOT_FOUND ) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorControlVariableShouldExist()


UNIT_TEST_STATUS
EFIAPI
MorControlVariableShouldHaveCorrectSize (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINTN         DataSize;
  UINT8         Data;

  DataSize = sizeof( Data );
  Status = gRT->GetVariable( MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                             &gEfiMemoryOverwriteControlDataGuid,
                             NULL,
                             &DataSize,
                             &Data );

  return (UT_ASSERT_NOT_EFI_ERROR( Status ) &&
          UT_ASSERT_EQUAL( DataSize, sizeof( Data ) )) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorControlVariableShouldHaveCorrectSize()


UNIT_TEST_STATUS
EFIAPI
MorControlVariableShouldHaveCorrectAttributes (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINT32        Attributes;
  UINTN         DataSize;
  UINT8         Data;

  DataSize = sizeof( Data );
  Status = gRT->GetVariable( MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                             &gEfiMemoryOverwriteControlDataGuid,
                             &Attributes,
                             &DataSize,
                             &Data );

  return (!EFI_ERROR( Status ) && Attributes == MOR_VARIABLE_ATTRIBUTES) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorControlVariableShouldHaveCorrectAttributes()


UNIT_TEST_STATUS
EFIAPI
MorControlShouldEnforceCorrectAttributes (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINT8             Data;
  UNIT_TEST_STATUS  Result = UNIT_TEST_ERROR_TEST_FAILED;

  // First, we have to try to delete the Control variable,
  // given that it should exist.
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                                &gEfiMemoryOverwriteControlDataGuid,
                                0,
                                0,
                                NULL );
  UT_ASSERT_NOT_EFI_ERROR( Status );

  // Then we want to try to set it with bad attributes.
  if (!EFI_ERROR( Status ))
  {
    Data    = FALSE;
    Status  = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                                &gEfiMemoryOverwriteControlDataGuid,
                                MOR_VARIABLE_BAD_ATTRIBUTES1,
                                sizeof( Data ),
                                &Data );
    if (Status == EFI_ACCESS_DENIED)
    {
      // TODO: This is my reminder to come back and finish this...
      // Result = UNIT_TEST_PASSED;
      UT_ASSERT_TRUE( TRUE == FALSE );
    }
  }

  // TODO: Write a MOR_VARIABLE_BAD_ATTRIBUTES2 test that works with auth.

  return Result;
} // MorControlShouldEnforceCorrectAttributes()


UNIT_TEST_STATUS
EFIAPI
MorControlShouldChangeWhenNotLocked (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINT8         MorControl;

  // Make sure that the variable can be set to TRUE.
  MorControl = TRUE;
  Status = SetMorControlVariable( &MorControl );
  if (!EFI_ERROR( Status ))
  {
    Status = GetMorControlVariable( &MorControl );
    if (!EFI_ERROR( Status) && MorControl != TRUE)
    {
      Status = EFI_ABORTED;
    }
  }

  // Make sure that the variable can be set to FALSE.
  if (!EFI_ERROR( Status ))
  {
    MorControl = FALSE;
    Status = SetMorControlVariable( &MorControl );
    if (!EFI_ERROR( Status ))
    {
      Status = GetMorControlVariable( &MorControl );
      if (!EFI_ERROR( Status) && MorControl != FALSE)
      {
        Status = EFI_ABORTED;
      }
    }
  }

  return !EFI_ERROR( Status ) ? UNIT_TEST_PASSED : UNIT_TEST_ERROR_TEST_FAILED;
} // MorControlShouldChangeWhenNotLocked()


UNIT_TEST_STATUS
EFIAPI
MorLockv1ShouldNotSetBadValue (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;
  UNIT_TEST_STATUS  Result = UNIT_TEST_PASSED;

  // Attempt to set the MorLock to a non-key, non-TRUE/FALSE value.
  MorLock   = 0xAA;
  DataSize  = sizeof( MorLock );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &MorLock );

  // Make sure that the status is EFI_INVALID_PARAMETER.
  if (!UT_ASSERT_STATUS_EQUAL( Status, EFI_INVALID_PARAMETER ))
  {
    Result = UNIT_TEST_ERROR_TEST_FAILED;
  }

  // If the value was accepted, for some reason,
  // we need to reboot to clean up after ourselves.
  if (!EFI_ERROR( Status ))
  {
    // Check this trick out...
    // We're gonna pull the rug out from under ourselves.
    ((UNIT_TEST_FRAMEWORK*)Framework)->CurrentTest->CleanUp = UnitTestCleanupReboot;
  }

  return Result;
} // MorLockv1ShouldNotSetBadValue()


UNIT_TEST_STATUS
EFIAPI
MorLockv1ShouldNotSetBadBufferSize (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock[] = { 0xDE, 0xAD, 0xBE, 0xEF };
  UNIT_TEST_STATUS  Result = UNIT_TEST_PASSED;

  // Attempt to set the MorLock to a non-key, non-TRUE/FALSE value.
  DataSize  = sizeof( MorLock );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &MorLock );

  // Make sure that the status is EFI_INVALID_PARAMETER.
  if (!UT_ASSERT_STATUS_EQUAL( Status, EFI_INVALID_PARAMETER ))
  {
    Result = UNIT_TEST_ERROR_TEST_FAILED;
  }

  // If the value was accepted, for some reason,
  // we need to reboot to clean up after ourselves.
  if (!EFI_ERROR( Status ))
  {
    // Check this trick out...
    // We're gonna pull the rug out fromm under ourselves.
    ((UNIT_TEST_FRAMEWORK*)Framework)->CurrentTest->CleanUp = UnitTestCleanupReboot;
  }

  return Result;
} // MorLockv1ShouldNotSetBadBufferSize()


UNIT_TEST_STATUS
EFIAPI
MorLockShouldNotSetBadAttributes (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;

  // Attempt to set the MorLock.
  DataSize  = sizeof( MorLock );
  MorLock   = TRUE;
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_BAD_ATTRIBUTES1,
                                DataSize,
                                &MorLock );

  return UT_ASSERT_STATUS_EQUAL( Status, EFI_INVALID_PARAMETER ) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockShouldNotSetBadAttributes()


UNIT_TEST_STATUS
EFIAPI
MorLockv1ShouldBeLockable (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;

  // Attempt to set the MorLock.
  DataSize  = sizeof( MorLock );
  MorLock   = TRUE;
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &MorLock );

  //
  // NOTE: Strictly speaking, this isn't a good unit test.
  //       After this test runs, the MorLock is set and the other tests
  //       have some expectation that the lock will behave a certain way.
  //       We *could* make better unit tests, but there would be a lot more
  //       reboots. So let's say this is for efficiency.
  //

  return UT_ASSERT_NOT_EFI_ERROR( Status ) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv1ShouldBeLockable()


UNIT_TEST_STATUS
EFIAPI
MorLockv1ShouldReportCorrectly (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINT8         MorLock;

  UT_LOG_VERBOSE( __FUNCTION__"()\n" );

  Status = GetMorLockVariable( &MorLock );

  UT_LOG_VERBOSE( __FUNCTION__" - Status = %r, MorLock = %d\n", Status, MorLock );

  return (UT_ASSERT_NOT_EFI_ERROR( Status ) &&
          UT_ASSERT_EQUAL( MorLock, MOR_LOCK_DATA_LOCKED_WITHOUT_KEY )) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv1ShouldReportCorrectly()


UNIT_TEST_STATUS
EFIAPI
MorControlShouldNotChange (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINT8             MorControl;
  UNIT_TEST_STATUS  Result = UNIT_TEST_PASSED;

  // Determine the current status.
  Status = GetMorControlVariable( &MorControl );
  if (EFI_ERROR( Status ))
  {
    return UNIT_TEST_ERROR_TEST_FAILED;
  }

  // If we're set, try to unset.
  if (MorControl)
  {
    MorControl = FALSE;
    Status = SetMorControlVariable( &MorControl );
    // If this was successful, that's not good.
    if (!EFI_ERROR( Status ))
    {
      Result = UNIT_TEST_ERROR_TEST_FAILED;
    }
  }
  // If we're unset, try to set.
  else
  {
    MorControl = TRUE;
    Status = SetMorControlVariable( &MorControl );
    // If this was successful, that's not good.
    if (!EFI_ERROR( Status ))
    {
      Result = UNIT_TEST_ERROR_TEST_FAILED;
    }
  }

  return Result;
} // MorControlShouldNotChange()


UNIT_TEST_STATUS
EFIAPI
MorLockv1ShouldNotChangeWhenLocked (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;

  // Attempt to unset the MorLock.
  DataSize  = sizeof( MorLock );
  MorLock   = FALSE;
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &MorLock );

  return UT_ASSERT_STATUS_EQUAL( Status, EFI_ACCESS_DENIED ) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv1ShouldNotChangeWhenLocked()


UNIT_TEST_STATUS
EFIAPI
MorLockv1ShouldNotBeDeleteable (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;

  // Attempt to delete the MorLock.
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                0,
                                0,
                                NULL );

  return UT_ASSERT_STATUS_EQUAL( Status, EFI_ACCESS_DENIED ) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv1ShouldNotBeDeleteable()


UNIT_TEST_STATUS
EFIAPI
MorLockShouldClearAfterReboot (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINT8             MorLock;
  UNIT_TEST_STATUS  Result = UNIT_TEST_ERROR_TEST_FAILED;
  BOOLEAN           IsPostReboot = FALSE;

  // Because we're going to reboot, we need to check for
  // a saved context.
  if (Context != NULL)
  {
    IsPostReboot = *(BOOLEAN*)Context;
  }

  // If we haven't reboot yet, we need to reboot.
  if (!IsPostReboot)
  {
    // Indicate that we've gotten here already...
    IsPostReboot = TRUE;

    UT_LOG_INFO( "Going down for reboot!\n" );
    // A warm reboot should be sufficient.
    SaveFrameworkStateAndReboot( Framework, &IsPostReboot, sizeof( IsPostReboot ), EfiResetWarm );
    // We shouldn't get here. If we do, we'll just return failure from this function.
    UT_LOG_ERROR( "Reboot failed! Should never get here!!\n" );
  }
  // Otherwise, we need to check the status of the MorLock.
  else
  {
    // Check the MorLock.
    Status = GetMorLockVariable( &MorLock );
    if (!EFI_ERROR( Status ) && MorLock == MOR_LOCK_DATA_UNLOCKED)
    {
      Result = UNIT_TEST_PASSED;
    }
  }

  return Result;
} // MorLockShouldClearAfterReboot()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldNotSetSmallBuffer (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF };
  UNIT_TEST_STATUS  Result = UNIT_TEST_PASSED;

  // First off, let's just make sure that our test is rational.
  DataSize = sizeof( MorLock );
  if (DataSize < MOR_LOCK_V2_KEY_SIZE + 1)
  {
    return UNIT_TEST_ERROR_TEST_FAILED;
  }

  // Attempt to set the MorLock to smaller than designated key size.
  DataSize = MOR_LOCK_V2_KEY_SIZE - 1;
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &MorLock );

  // Make sure that the status is EFI_INVALID_PARAMETER.
  if (!UT_ASSERT_STATUS_EQUAL( Status, EFI_INVALID_PARAMETER ))
  {
    Result = UNIT_TEST_ERROR_TEST_FAILED;
  }

  // If the value was accepted, for some reason,
  // we need to reboot to clean up after ourselves.
  if (!EFI_ERROR( Status ))
  {
    // Check this trick out...
    // We're gonna pull the rug out fromm under ourselves.
    ((UNIT_TEST_FRAMEWORK*)Framework)->CurrentTest->CleanUp = UnitTestCleanupReboot;
  }

  return Result;
} // MorLockv2ShouldNotSetSmallBuffer()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldNotSetLargeBuffer (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF };
  UNIT_TEST_STATUS  Result = UNIT_TEST_PASSED;

  // First off, let's just make sure that our test is rational.
  DataSize = sizeof( MorLock );
  if (DataSize < MOR_LOCK_V2_KEY_SIZE + 1)
  {
    return UNIT_TEST_ERROR_TEST_FAILED;
  }

  // Attempt to set the MorLock to larger than designated key size.
  DataSize = MOR_LOCK_V2_KEY_SIZE + 1;
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &MorLock );

  // Make sure that the status is EFI_INVALID_PARAMETER.
  if (!UT_ASSERT_STATUS_EQUAL( Status, EFI_INVALID_PARAMETER ))
  {
    Result = UNIT_TEST_ERROR_TEST_FAILED;
  }

  // If the value was accepted, for some reason,
  // we need to reboot to clean up after ourselves.
  if (!EFI_ERROR( Status ))
  {
    // Check this trick out...
    // We're gonna pull the rug out fromm under ourselves.
    ((UNIT_TEST_FRAMEWORK*)Framework)->CurrentTest->CleanUp = UnitTestCleanupReboot;
  }

  return Result;
} // MorLockv2ShouldNotSetLargeBuffer()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldNotSetNoBuffer (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;
  UNIT_TEST_STATUS  Result = UNIT_TEST_PASSED;

  // Attempt to set the MorLock v2 directly;
  DataSize  = sizeof( MorLock );
  MorLock   = MOR_LOCK_DATA_LOCKED_WITH_KEY;
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &MorLock );

  // Make sure that the status is EFI_INVALID_PARAMETER.
  if (!UT_ASSERT_STATUS_EQUAL( Status, EFI_INVALID_PARAMETER ))
  {
    Result = UNIT_TEST_ERROR_TEST_FAILED;
  }

  // If the value was accepted, for some reason,
  // we need to reboot to clean up after ourselves.
  if (!EFI_ERROR( Status ))
  {
    // Check this trick out...
    // We're gonna pull the rug out fromm under ourselves.
    ((UNIT_TEST_FRAMEWORK*)Framework)->CurrentTest->CleanUp = UnitTestCleanupReboot;
  }

  return Result;
} // MorLockv2ShouldNotSetNoBuffer()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldBeLockable (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;

  // Attempt to set a key for MorLock v2.
  // For this test, we'll use Test Key 1.
  DataSize  = sizeof( mTestKey1 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey1[0] );

  //
  // NOTE: Strictly speaking, this isn't a good unit test.
  //       After this test runs, the MorLock is set and the other tests
  //       have some expectation that the lock will behave a certain way.
  //       We *could* make better unit tests, but there would be a lot more
  //       reboots. So let's say this is for efficiency.
  //

  return UT_ASSERT_NOT_EFI_ERROR( Status ) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv2ShouldBeLockable()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldReportCorrectly (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINT8         MorLock;

  UT_LOG_VERBOSE( __FUNCTION__"()\n" );

  Status = GetMorLockVariable( &MorLock );

  UT_LOG_VERBOSE( __FUNCTION__" - Status = %r, MorLock = %d\n", Status, MorLock );

  return (UT_ASSERT_NOT_EFI_ERROR( Status ) &&
          UT_ASSERT_EQUAL( MorLock, MOR_LOCK_DATA_LOCKED_WITH_KEY )) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv2ShouldReportCorrectly()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldOnlyReturnOneByte (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINTN         DataSize;
  UINT8         MorLock[MOR_LOCK_V2_KEY_SIZE];

  // Blank the buffer so we know it doesn't contain the key.
  ZeroMem( &MorLock[0], sizeof( MorLock ) );

  // Fetch the MorLock so we can see what we get.
  DataSize = sizeof( MorLock );
  Status = gRT->GetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                             &gEfiMemoryOverwriteRequestControlLockGuid,
                             NULL,
                             &DataSize,
                             &MorLock );

  // Check to see how much data was returned.
  if (!EFI_ERROR( Status ) && DataSize > 1)
  {
    Status = EFI_ACCESS_DENIED;
  }

  return (!EFI_ERROR( Status )) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv2ShouldOnlyReturnOneByte()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldNotReturnKey (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS    Status;
  UINTN         DataSize;
  UINT8         MorLock[MOR_LOCK_V2_KEY_SIZE];

  // Blank the buffer so we know it doesn't contain the key.
  ZeroMem( &MorLock[0], sizeof( MorLock ) );

  // Fetch the MorLock so we can see what we get.
  DataSize = sizeof( MorLock );
  Status = gRT->GetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                             &gEfiMemoryOverwriteRequestControlLockGuid,
                             NULL,
                             &DataSize,
                             &MorLock );

  // Check for the key in the buffer.
  // We would EXPECT to only receive one byte, but you never know.
  if (!EFI_ERROR( Status ) && DataSize > 1)
  {
    // We're only using three keys in these tests.
    // Might as well check them all.
    if (CompareMem( &MorLock[0], &mTestKey1[0], sizeof( MorLock ) ) == 0 ||
        CompareMem( &MorLock[0], &mTestKey2[0], sizeof( MorLock ) ) == 0 ||
        CompareMem( &MorLock[0], &mTestKey3[0], sizeof( MorLock ) ) == 0)
    {
      Status = EFI_ACCESS_DENIED;
    }
  }

  return (!EFI_ERROR( Status )) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv2ShouldNotReturnKey()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldNotChangeWhenLocked (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;

  // Attempt to change the key for MorLock v2.
  // For this test, we'll use Test Key 2.
  DataSize  = sizeof( mTestKey2 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey2[0] );

  return UT_ASSERT_STATUS_EQUAL( Status, EFI_ACCESS_DENIED ) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv2ShouldNotChangeWhenLocked()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldNotChangeTov1 (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;

  // Attempt to set the MorLock to v1.
  DataSize  = sizeof( MorLock );
  MorLock   = TRUE;
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &MorLock );

  return UT_ASSERT_STATUS_EQUAL( Status, EFI_ACCESS_DENIED ) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv2ShouldNotChangeTov1()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldNotBeDeleteable (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;

  // Attempt to delete the MorLock.
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                0,
                                0,
                                NULL );

  return UT_ASSERT_STATUS_EQUAL( Status, EFI_ACCESS_DENIED ) ?
         UNIT_TEST_PASSED :
         UNIT_TEST_ERROR_TEST_FAILED;
} // MorLockv2ShouldNotBeDeleteable()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldClearWithCorrectKey (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;
  UNIT_TEST_STATUS  Result = UNIT_TEST_ERROR_TEST_FAILED;

  // Attempt to set a key for MorLock v2.
  // For this test, we'll use Test Key 1.
  DataSize  = sizeof( mTestKey1 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey1[0] );
  // If this key failed to set, we're done here.
  if (EFI_ERROR( Status ))
  {
    goto Exit;
  }

  // Verify that the key was set.
  Status = GetMorLockVariable( &MorLock );
  if (EFI_ERROR( Status ) || MorLock != MOR_LOCK_DATA_LOCKED_WITH_KEY)
  {
    goto Exit;
  }

  // Attempt to clear with the same key.
  DataSize  = sizeof( mTestKey1 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey1[0] );
  // If this key failed to set, we're done here.
  if (EFI_ERROR( Status ))
  {
    goto Exit;
  }

  // Verify that mode is now disabled.
  Status = GetMorLockVariable( &MorLock );
  if (EFI_ERROR( Status ) || MorLock != MOR_LOCK_DATA_UNLOCKED)
  {
    goto Exit;
  }

  // Only if we've made it this far are we good to return UNIT_TEST_PASSED.
  Result = UNIT_TEST_PASSED;

Exit:
  return Result;
} // MorLockv2ShouldClearWithCorrectKey()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldNotClearWithWrongKey (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;
  UNIT_TEST_STATUS  Result = UNIT_TEST_ERROR_TEST_FAILED;

  // Attempt to set a key for MorLock v2.
  // For this test, we'll use Test Key 1.
  DataSize  = sizeof( mTestKey1 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey1[0] );
  // If this key failed to set, we're done here.
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ))
  {
    goto Exit;
  }

  // Verify that the key was set.
  Status = GetMorLockVariable( &MorLock );
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ) || !UT_ASSERT_EQUAL( MorLock, MOR_LOCK_DATA_LOCKED_WITH_KEY ))
  {
    goto Exit;
  }

  // Attempt to clear with a different key.
  DataSize  = sizeof( mTestKey2 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey2[0] );
  // If this key was successfully set, we're done here.
  if (!UT_ASSERT_STATUS_EQUAL( Status, EFI_ACCESS_DENIED ))
  {
    goto Exit;
  }

  // Verify that mode is still enabled.
  Status = GetMorLockVariable( &MorLock );
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ) || !UT_ASSERT_EQUAL( MorLock, MOR_LOCK_DATA_LOCKED_WITH_KEY ))
  {
    goto Exit;
  }

  // Only if we've made it this far are we good to return UNIT_TEST_PASSED.
  Result = UNIT_TEST_PASSED;

Exit:
  return Result;
} // MorLockv2ShouldNotClearWithWrongKey()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldReleaseMorControlAfterClear (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;
  UNIT_TEST_STATUS  Result = UNIT_TEST_ERROR_TEST_FAILED;

  // Attempt to set a key for MorLock v2.
  // For this test, we'll use Test Key 1.
  DataSize  = sizeof( mTestKey1 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey1[0] );
  // If this key failed to set, we're done here.
  if (EFI_ERROR( Status ))
  {
    goto Exit;
  }

  // Verify that the key was set.
  Status = GetMorLockVariable( &MorLock );
  if (EFI_ERROR( Status ) || MorLock != MOR_LOCK_DATA_LOCKED_WITH_KEY)
  {
    goto Exit;
  }

  // Attempt to clear with the same key.
  DataSize  = sizeof( mTestKey1 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey1[0] );
  // If this key failed to set, we're done here.
  if (EFI_ERROR( Status ))
  {
    goto Exit;
  }

  // Verify that mode is now disabled.
  Status = GetMorLockVariable( &MorLock );
  if (EFI_ERROR( Status ) || MorLock != MOR_LOCK_DATA_UNLOCKED)
  {
    goto Exit;
  }

  // If we've made it this far, the only thing left to do is make sure
  // that the MOR Control can change.
  Result = MorControlShouldChangeWhenNotLocked( Framework, NULL );

Exit:
  return Result;
} // MorLockv2ShouldReleaseMorControlAfterClear()


UNIT_TEST_STATUS
EFIAPI
MorLockv2ShouldSetClearSet (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS        Status;
  UINTN             DataSize;
  UINT8             MorLock;
  UNIT_TEST_STATUS  Result = UNIT_TEST_ERROR_TEST_FAILED;

  //
  // Attempt to set a key for MorLock v2.
  // For this test, we'll use Test Key 1.
  //
  DataSize  = sizeof( mTestKey1 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey1[0] );
  // If this key failed to set, we're done here.
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ))
  {
    goto Exit;
  }

  // Verify that the key was set.
  Status = GetMorLockVariable( &MorLock );
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ) || !UT_ASSERT_EQUAL( MorLock, MOR_LOCK_DATA_LOCKED_WITH_KEY ))
  {
    goto Exit;
  }

  //
  // Attempt to clear with the same key.
  //
  DataSize  = sizeof( mTestKey1 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey1[0] );
  // If this key failed to set, we're done here.
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ))
  {
    goto Exit;
  }

  // Verify that mode is now disabled.
  Status = GetMorLockVariable( &MorLock );
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ) || !UT_ASSERT_EQUAL( MorLock, MOR_LOCK_DATA_UNLOCKED ))
  {
    goto Exit;
  }

  //
  // Attempt to set a second key.
  //
  DataSize  = sizeof( mTestKey2 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey2[0] );
  // If this key failed to set, we're done here.
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ))
  {
    goto Exit;
  }

  // Verify that the key was set.
  Status = GetMorLockVariable( &MorLock );
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ) || !UT_ASSERT_EQUAL( MorLock, MOR_LOCK_DATA_LOCKED_WITH_KEY ))
  {
    goto Exit;
  }

  //
  // Attempt to clear with a different key.
  //
  DataSize  = sizeof( mTestKey3 );
  Status    = gRT->SetVariable( MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
                                &gEfiMemoryOverwriteRequestControlLockGuid,
                                MOR_VARIABLE_ATTRIBUTES,
                                DataSize,
                                &mTestKey3[0] );
  // If this key was successfully set, we're done here.
  if (!UT_ASSERT_STATUS_EQUAL( Status, EFI_ACCESS_DENIED ))
  {
    goto Exit;
  }

  // Verify that mode is still enabled.
  Status = GetMorLockVariable( &MorLock );
  if (!UT_ASSERT_NOT_EFI_ERROR( Status ) || !UT_ASSERT_EQUAL( MorLock, MOR_LOCK_DATA_LOCKED_WITH_KEY ))
  {
    goto Exit;
  }

  //
  // Only if we've made it this far are we good to return UNIT_TEST_PASSED.
  //
  Result = UNIT_TEST_PASSED;

Exit:
  return Result;
} // MorLockv2ShouldSetClearSet()


///================================================================================================
///================================================================================================
///
/// TEST ENGINE
///
///================================================================================================
///================================================================================================


/**
  MorLockTestApp
  
  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point executed successfully.
  @retval other           Some error occured when executing this entry point.

**/
EFI_STATUS
EFIAPI
MorLockTestApp (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                Status;
  UNIT_TEST_FRAMEWORK       *Fw = NULL;
  UNIT_TEST_SUITE           *EnvironmentalTests, *MorLockV1Tests, *MorLockV2Tests;
  BOOLEAN                   TestsRun = FALSE;

  DEBUG(( DEBUG_INFO, "%s v%s\n", UNIT_TEST_APP_NAME, UNIT_TEST_APP_VERSION ));

  //
  // Start setting up the test framework for running the tests.
  //
  Status = InitUnitTestFramework( &Fw, UNIT_TEST_APP_NAME, UNIT_TEST_APP_SHORT_NAME, UNIT_TEST_APP_VERSION );
  if (EFI_ERROR( Status ))
  {
    DEBUG((DEBUG_ERROR, "Failed in InitUnitTestFramework. Status = %r\n", Status));
    goto EXIT;
  }

  //
  // Populate the EnvironmentalTests Unit Test Suite.
  //
  Status = CreateUnitTestSuite( &EnvironmentalTests, Fw, L"Boot Environment Tests", NULL, NULL );
  if (EFI_ERROR( Status ))
  {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for EnvironmentalTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }
  AddTestCase( EnvironmentalTests, L"On any given boot, the MOR control variable should exist", MorControlVariableShouldExist, NULL, NULL, NULL );
  AddTestCase( EnvironmentalTests, L"MOR control variable should be the correct size", MorControlVariableShouldHaveCorrectSize, NULL, NULL, NULL );
  AddTestCase( EnvironmentalTests, L"MOR control variable should have correct attributes", MorControlVariableShouldHaveCorrectAttributes, NULL, NULL, NULL );
  AddTestCase( EnvironmentalTests, L"Should not be able to create MOR control variable with incorrect attributes", MorControlShouldEnforceCorrectAttributes, NULL, UnitTestCleanupReboot, NULL );

  // IMPORTANT NOTE: On a reboot test, currently, prereqs will be run each time the test is continued. Ergo, a prereq that may be
  //                 valid on a single boot may not be valid on subsequent boots. THIS MUST BE SOLVED!!

  //
  // Populate the MorLockV1Tests Unit Test Suite.
  //
  Status = CreateUnitTestSuite( &MorLockV1Tests, Fw, L"MORLock v1 Tests", NULL, NULL );
  if (EFI_ERROR( Status ))
  {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for MorLockV1Tests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }
  AddTestCase( MorLockV1Tests, L"Should be able to change MOR control when not locked", MorControlShouldChangeWhenNotLocked, MorControlVariableShouldBeCorrect, NULL, NULL );
  AddTestCase( MorLockV1Tests, L"Should not be able to set MORLock v1 with a bad value", MorLockv1ShouldNotSetBadValue, MorLockShouldNotBeSet, NULL, NULL );
  AddTestCase( MorLockV1Tests, L"Should not be able to set MORLock v1 with strange buffer size", MorLockv1ShouldNotSetBadBufferSize, MorLockShouldNotBeSet, NULL, NULL );
  AddTestCase( MorLockV1Tests, L"Should not be able to set MORLock v1 with bad attributes", MorLockShouldNotSetBadAttributes, MorLockShouldNotBeSet, NULL, NULL );
  //
  // NOTE: Strictly speaking, this isn't a good unit test.
  //       After this test runs, the MorLock is set and the other tests
  //       have some expectation that the lock will behave a certain way.
  //       We *could* make better unit tests, but there would be a lot more
  //       reboots. So let's say this is for efficiency.
  //
  AddTestCase( MorLockV1Tests, L"Should be able to set the v1 MORLock", MorLockv1ShouldBeLockable, MorLockShouldNotBeSet, NULL, NULL );
  AddTestCase( MorLockV1Tests, L"Should report version correctly when locked with MORLock v1", MorLockv1ShouldReportCorrectly, NULL, NULL, NULL );
  AddTestCase( MorLockV1Tests, L"Should not be able to change the MOR control when locked with MORLock v1", MorControlShouldNotChange, MorLockv1ShouldReportCorrectly, NULL, NULL );
  AddTestCase( MorLockV1Tests, L"Should not be able to change the MORLock when locked with MORLock v1", MorLockv1ShouldNotChangeWhenLocked, MorLockv1ShouldReportCorrectly, NULL, NULL );
  AddTestCase( MorLockV1Tests, L"Should not be able to delete the MORLock when locked with MORLock v1", MorLockv1ShouldNotBeDeleteable, MorLockv1ShouldReportCorrectly, NULL, NULL );
  AddTestCase( MorLockV1Tests, L"MORLock v1 should clear after reboot", MorLockShouldClearAfterReboot, MorLockv1ShouldReportCorrectly, NULL, NULL );

  //
  // Populate the MorLockV2Tests Unit Test Suite.
  //
  Status = CreateUnitTestSuite( &MorLockV2Tests, Fw, L"MORLock v2 Tests", NULL, NULL );
  if (EFI_ERROR( Status ))
  {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for MorLockV2Tests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }
  AddTestCase( MorLockV2Tests, L"Should be able to change MOR control when not locked", MorControlShouldChangeWhenNotLocked, MorControlVariableShouldBeCorrect, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should not be able to set MORLock v2 with buffer too small", MorLockv2ShouldNotSetSmallBuffer, MorLockShouldNotBeSet, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should not be able to set MORLock v2 with buffer too large", MorLockv2ShouldNotSetLargeBuffer, MorLockShouldNotBeSet, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should not be able to set MORLock v2 without a key", MorLockv2ShouldNotSetNoBuffer, MorLockShouldNotBeSet, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should not be able to set MORLock v2 with bad attributes", MorLockShouldNotSetBadAttributes, MorLockShouldNotBeSet, NULL, NULL );
  //
  // NOTE: Strictly speaking, this isn't a good unit test.
  //       After this test runs, the MorLock is set and the other tests
  //       have some expectation that the lock will behave a certain way.
  //       We *could* make better unit tests, but there would be a lot more
  //       reboots. So let's say this is for efficiency.
  //
  AddTestCase( MorLockV2Tests, L"Should be able to set the v2 MORLock", MorLockv2ShouldBeLockable, MorLockShouldNotBeSet, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should report version correctly when locked with MORLock v2", MorLockv2ShouldReportCorrectly, NULL, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should only return one byte when reading MORLock v2", MorLockv2ShouldOnlyReturnOneByte, MorLockv2ShouldReportCorrectly, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should not return the key contents when locked with MORLock v2", MorLockv2ShouldNotReturnKey, MorLockv2ShouldReportCorrectly, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should not be able to change the MOR control when locked with MORLock v2", MorControlShouldNotChange, MorLockv2ShouldReportCorrectly, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should not be able to change the key when locked with MORLock v2", MorLockv2ShouldNotChangeWhenLocked, MorLockv2ShouldReportCorrectly, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should not be able to change to MORLock v1 when locked with MORLock v2", MorLockv2ShouldNotChangeTov1, MorLockv2ShouldReportCorrectly, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"Should not be able to delete the MORLock when locked with MORLock v2", MorLockv2ShouldNotBeDeleteable, MorLockv2ShouldReportCorrectly, NULL, NULL );
  AddTestCase( MorLockV2Tests, L"MORLock v2 should clear after reboot", MorLockShouldClearAfterReboot, MorLockv2ShouldReportCorrectly, NULL, NULL );
  //
  // End of tests that assume precedence.
  // From here on, each test is isolated and will clean up after itself.
  //
  AddTestCase( MorLockV2Tests, L"MORLock v2 should clear with a correct key", MorLockv2ShouldClearWithCorrectKey, MorLockShouldNotBeSet, UnitTestCleanupReboot, NULL );
  AddTestCase( MorLockV2Tests, L"MORLock v2 should not clear with an incorrect key", MorLockv2ShouldNotClearWithWrongKey, MorLockShouldNotBeSet, UnitTestCleanupReboot, NULL );
  AddTestCase( MorLockV2Tests, L"Should be able to change MOR control after setting and clearing MORLock v2", MorLockv2ShouldReleaseMorControlAfterClear, MorLockShouldNotBeSet, UnitTestCleanupReboot, NULL );
  AddTestCase( MorLockV2Tests, L"Should be able to change keys by setting, clearing, and setting MORLock v2", MorLockv2ShouldSetClearSet, MorLockShouldNotBeSet, UnitTestCleanupReboot, NULL );

  //
  // Execute the tests.
  //
  TestsRun = TRUE;
  Status = RunAllTestSuites( Fw );

EXIT:
  if (TestsRun)
  {
    PrintUnitTestReport( Fw );
  }

  if (Fw)
  {
    FreeUnitTestFramework( Fw );
  }

  return Status;
}
