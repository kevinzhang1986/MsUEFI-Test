##
# Partial DSC to be included for the UnitTest build
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
#    THE POSSIBILITY OF SUCH DAMAGE.
#
#    
#    Copyright (C) 2016 Microsoft Corporation. All Rights Reserved.
##

#Sample
MsUnitTestPkg/SampleUnitTestApp/SampleUnitTestApp.inf {
  <LibraryClasses>
    NULL|MsUnitTestPkg\Library\UnitTestLib\UnitTestNullPersistenceLib.inf
}

# MemMap and MAT Test
MsUnitTestPkg/MemmapAndMatTestApp/MemmapAndMatTestApp.inf {
  <LibraryClasses>
    NULL|MsUnitTestPkg\Library\UnitTestLib\UnitTestNullPersistenceLib.inf
}

# MorLock v1 and v2 Test
MsUnitTestPkg/MorLockTestApp/MorLockTestApp.inf {
  <LibraryClasses>
    ## Since this test requires a reboot, include a library to persist the data.
    NULL|MsUnitTestPkg\Library\UnitTestLib\UnitTestFilesystemPersistenceLib.inf
}
