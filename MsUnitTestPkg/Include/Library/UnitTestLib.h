/** @file
Provides a unit test framework.  This allows tests to focus on testing logic
and the framework to focus on runnings, reporting, statistics, etc. 

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

#ifndef __UNIT_TEST_LIB_H__
#define __UNIT_TEST_LIB_H__

///================================================================================================
///================================================================================================
///
/// HANDY DEFINITIONS
///
///================================================================================================
///================================================================================================


#define UNIT_TEST_MAX_STRING_LENGTH               (120)
#define UNIT_TEST_MAX_SINGLE_LOG_STRING_LENGTH    (512)

#define UNIT_TEST_FINGERPRINT_SIZE  16    // Hardcoded to MD5_HASHSIZE.

typedef UINT32 UNIT_TEST_STATUS;
#define UNIT_TEST_PASSED                      (0)
#define UNIT_TEST_ERROR_PREREQ_NOT_MET        (1)
#define UNIT_TEST_ERROR_TEST_FAILED           (2)
#define UNIT_TEST_RUNNING                     (0xFFFFFFFE)
#define UNIT_TEST_PENDING                     (0xFFFFFFFF)

#define DEBUG_UT_VERBOSE     0x010000000  // Unit Test Verbose
#define DEBUG_UT_INFO        0x020000000  // Unit Test Info
#define DEBUG_UT_WARNING     0x040000000  // Unit Test Warning
#define DEBUG_UT_ERROR       0x080000000  // Unit Test Error

typedef VOID*   UNIT_TEST_FRAMEWORK_HANDLE; // Same as a UNIT_TEST_FRAMEWORK*, but with fewer build errors.
typedef VOID*   UNIT_TEST_SUITE_HANDLE;     // Same as a UNIT_TEST_SUITE*, but with fewer build errors.
typedef VOID*   UNIT_TEST_CONTEXT;


///================================================================================================
///================================================================================================
///
/// UNIT TEST FUNCTION TYPE DEFINITIONS
///
///================================================================================================
///================================================================================================


//
// Unit-Test Function pointer type.
//
typedef
UNIT_TEST_STATUS
(EFIAPI *UNIT_TEST_FUNCTION) (
  UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  UNIT_TEST_CONTEXT           Context
  );

//
// Unit-Test Prerequisite Function pointer type.
// NOTE: Should be the same as UnitTest.
//
typedef
UNIT_TEST_STATUS
(EFIAPI *UNIT_TEST_PREREQ) (
  UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  UNIT_TEST_CONTEXT           Context
  );

//
// Unit-Test Test Cleanup (after) function pointer type.
//
typedef
VOID
(EFIAPI *UNIT_TEST_CLEANUP) (
  UNIT_TEST_FRAMEWORK_HANDLE  Framework
  );

//
// Unit-Test Test Suite Setup (before) function pointer type.
//
typedef
VOID
(EFIAPI *UNIT_TEST_SUITE_SETUP) (
  UNIT_TEST_FRAMEWORK_HANDLE  Framework
  );

//
// Unit-Test Test Suite Teardown (after) function pointer type.
//
typedef
VOID
(EFIAPI *UNIT_TEST_SUITE_TEARDOWN) (
  UNIT_TEST_FRAMEWORK_HANDLE  Framework
  );


///================================================================================================
///================================================================================================
///
/// UNIT TEST DATA STRUCTURE DEFINITIONS
///
///================================================================================================
///================================================================================================


typedef struct {
  CHAR16                    *Description;
  CHAR16                    *Log;
  UINT8                     Fingerprint[UNIT_TEST_FINGERPRINT_SIZE];
  UNIT_TEST_STATUS          Result;
  UNIT_TEST_FUNCTION        RunTest;
  UNIT_TEST_PREREQ          PreReq;
  UNIT_TEST_CLEANUP         CleanUp;
  UNIT_TEST_CONTEXT         Context;
  UNIT_TEST_SUITE_HANDLE    ParentSuite;
} UNIT_TEST;

typedef struct {
  LIST_ENTRY    Entry;
  UNIT_TEST     UT;
} UNIT_TEST_LIST_ENTRY;

typedef struct {
  CHAR16                      *Title;
  UINT8                       Fingerprint[UNIT_TEST_FINGERPRINT_SIZE];
  UNIT_TEST_SUITE_SETUP       Setup;
  UNIT_TEST_SUITE_TEARDOWN    Teardown;
  LIST_ENTRY                  TestCaseList;     // UNIT_TEST_LIST_ENTRY
  UNIT_TEST_FRAMEWORK_HANDLE  ParentFramework;
} UNIT_TEST_SUITE;

typedef struct {
  LIST_ENTRY        Entry;
  UNIT_TEST_SUITE   UTS;
} UNIT_TEST_SUITE_LIST_ENTRY;

typedef struct {
  CHAR16                    *Title;
  CHAR16                    *ShortTitle;      // This title should contain NO spaces or non-filename charatecters. Is used in reporting and serialization.
  CHAR16                    *VersionString;
  CHAR16                    *Log;
  UINT8                     Fingerprint[UNIT_TEST_FINGERPRINT_SIZE];
  LIST_ENTRY                TestSuiteList;    // UNIT_TEST_SUITE_LIST_ENTRY
  EFI_TIME                  StartTime;
  EFI_TIME                  EndTime;
  UNIT_TEST                 *CurrentTest;
  VOID                      *SavedState;      // This is an instance of UNIT_TEST_SAVE_HEADER*, if present.
} UNIT_TEST_FRAMEWORK;


///================================================================================================
///================================================================================================
///
/// UNIT TEST LIBRARY FUNCTION PROTOTYPES
///
///================================================================================================
///================================================================================================


/*
Method to Initialize the Unit Test framework

@retval Success - Unit Test init.
@retval EFI_ERROR - Unit Tests init failed.  
*/
EFI_STATUS
EFIAPI
InitUnitTestFramework (
  OUT UNIT_TEST_FRAMEWORK   **Framework,
  IN  CHAR16                *Title,
  IN  CHAR16                *ShortTitle,
  IN  CHAR16                *VersionString
  );


/*
Method to print the Unit Test run results

@retval Success
*/
EFI_STATUS
EFIAPI
PrintUnitTestReport(
  IN UNIT_TEST_FRAMEWORK  *Framework
);

EFI_STATUS
EFIAPI
CreateUnitTestSuite (
  OUT UNIT_TEST_SUITE           **Suite,
  IN UNIT_TEST_FRAMEWORK        *Framework,
  IN CHAR16                     *Title,
  IN UNIT_TEST_SUITE_SETUP      Sup    OPTIONAL,
  IN UNIT_TEST_SUITE_TEARDOWN   Tdn    OPTIONAL
  );

EFI_STATUS
EFIAPI
AddTestCase (
  IN UNIT_TEST_SUITE      *Suite,
  IN CHAR16               *Description,
  IN UNIT_TEST_FUNCTION   Func,
  IN UNIT_TEST_PREREQ     PreReq    OPTIONAL,
  IN UNIT_TEST_CLEANUP    CleanUp   OPTIONAL,
  IN UNIT_TEST_CONTEXT    Context   OPTIONAL
  );

EFI_STATUS
EFIAPI
RunAllTestSuites(
  IN UNIT_TEST_FRAMEWORK  *Framework
  );

EFI_STATUS
EFIAPI
FreeUnitTestFramework (
  IN UNIT_TEST_FRAMEWORK  *Framework
  );

EFI_STATUS
EFIAPI
SaveFrameworkState (
  IN UNIT_TEST_FRAMEWORK_HANDLE FrameworkHandle,
  IN UNIT_TEST_CONTEXT          ContextToSave     OPTIONAL,
  IN UINTN                      ContextToSaveSize
  );

EFI_STATUS
EFIAPI
SaveFrameworkStateAndQuit (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN UNIT_TEST_CONTEXT          ContextToSave     OPTIONAL,
  IN UINTN                      ContextToSaveSize
  );

/**
  NOTE: Takes in a ResetType, but currently only supports EfiResetCold
        and EfiResetWarm. All other types will return EFI_INVALID_PARAMETER.
        If a more specific reset is required, use SaveFrameworkState() and
        call gRT->ResetSystem() directly.

**/
EFI_STATUS
EFIAPI
SaveFrameworkStateAndReboot (
  IN UNIT_TEST_FRAMEWORK_HANDLE FrameworkHandle,
  IN UNIT_TEST_CONTEXT          ContextToSave     OPTIONAL,
  IN UINTN                      ContextToSaveSize,
  IN EFI_RESET_TYPE             ResetType
  );


///================================================================================================
///================================================================================================
///
/// UNIT TEST LOGGING DEFINITIONS AND FUNCTIONS
///
///================================================================================================
///================================================================================================


// IMPORTANT NOTE: These macros should ONLY be used in a Unit Test.
//                 They will consume the Framework Handle and update the Framework->CurrentTest.

#define UT_LOG_ERROR(Format, ...)              \
  UnitTestLog( Framework, DEBUG_ERROR, Format, __VA_ARGS__ );
#define UT_LOG_WARNING(Format, ...)            \
  UnitTestLog( Framework, DEBUG_WARN, Format, __VA_ARGS__ );
#define UT_LOG_INFO(Format, ...)               \
  UnitTestLog( Framework, DEBUG_INFO, Format, __VA_ARGS__ );
#define UT_LOG_VERBOSE(Format, ...)            \
  UnitTestLog( Framework, DEBUG_VERBOSE, Format, __VA_ARGS__ );

VOID
EFIAPI
UnitTestLog (
  IN  UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN  UINTN                       ErrorLevel,
  IN  CONST CHAR8                 *Format,
  ...
  );

// NOTE: Thoughts...
//        *Could* pass the framework in and actually update the test status if one of these assertions fails.
//        In this way, a unit test wouldn't have to return a status; it would just make one or more assertions.
//        If any of these assertions fails, the test will automatically be marked as a failure.
//
//        For now, I think we'll just have these return TRUE or FALSE so that we can use if()...
//        and update the test status based of of the if().

// TODO: UT_CUSTOM_FAILURE

#define UT_ASSERT_TRUE(Expression)                \
  UnitTestAssertTrue( Framework, (Expression), __FUNCTION__, __LINE__, #Expression )

#define UT_ASSERT_FALSE(Expression)               \
  UnitTestAssertFalse( Framework, (Expression), __FUNCTION__, __LINE__, #Expression )

#define UT_ASSERT_EQUAL(ValueA, ValueB)           \
  UnitTestAssertEqual( Framework, ValueA, ValueB, __FUNCTION__, __LINE__, #ValueA, #ValueB )

#define UT_ASSERT_NOT_EQUAL(ValueA, ValueB)       \
  UnitTestAssertNotEqual( Framework, ValueA, ValueB, __FUNCTION__, __LINE__, #ValueA, #ValueB )

#define UT_ASSERT_NOT_EFI_ERROR(Status)           \
  UnitTestAssertNotEfiError( Framework, Status, __FUNCTION__, __LINE__, #Status )

#define UT_ASSERT_STATUS_EQUAL(Status, Expected)  \
  UnitTestAssertStatusEqual( Framework, Status, Expected, __FUNCTION__, __LINE__, #Status )

BOOLEAN
EFIAPI
UnitTestAssertTrue (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN BOOLEAN                    Expression,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *Description
  );

BOOLEAN
EFIAPI
UnitTestAssertFalse (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN BOOLEAN                    Expression,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *Description
  );

BOOLEAN
EFIAPI
UnitTestAssertNotEfiError (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN EFI_STATUS                 Status,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *Description
  );

BOOLEAN
EFIAPI
UnitTestAssertEqual (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN UINTN                      ValueA,
  IN UINTN                      ValueB,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *DescriptionA,
  IN CONST CHAR8                *DescriptionB
  );

BOOLEAN
EFIAPI
UnitTestAssertNotEqual (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN UINTN                      ValueA,
  IN UINTN                      ValueB,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *DescriptionA,
  IN CONST CHAR8                *DescriptionB
  );

BOOLEAN
EFIAPI
UnitTestAssertStatusEqual (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN EFI_STATUS                 Status,
  IN EFI_STATUS                 Expected,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *Description
  );

#endif