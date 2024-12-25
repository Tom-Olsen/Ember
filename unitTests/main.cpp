#include <gtest/gtest.h>

// mathf testing:
#include "mathf.h"
#include "testBounds.h"
#include "testFloat2.h"
#include "testFloat2x2.h"
#include "testFloat2x3.h"
#include "testFloat3.h"
#include "testFloat3x2.h"
#include "testFloat3x3.h"
#include "testFloat4.h"
#include "testFloat4x4.h"
#include "testGeometry3d.h"
#include "testInt2.h"
#include "testInt3.h"
#include "testUint3.h"



int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}