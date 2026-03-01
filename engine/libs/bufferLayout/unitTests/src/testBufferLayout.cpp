#include "logger.h"
#include "bufferLayout.h"
#include <gtest/gtest.h>



using namespace emberBufferLayout;



struct alignas(16) Foo
{
	int a;
	float b;
};



TEST(ShaderReflection, VertexShaderReflection)
{
	try
	{
		BufferLayout bufferLayout("testLayout");

		BufferMember memberFoo("foo", 0, sizeof(Foo));
		BufferMember subMemberFooA("a", 0, sizeof(int));
		BufferMember subMemberFooB("b", sizeof(int), sizeof(float));
		memberFoo.AddSubMember(subMemberFooA);
		memberFoo.AddSubMember(subMemberFooB);
		bufferLayout.AddMember(memberFoo);

		BufferMember memberC("c", sizeof(Foo), 4 * sizeof(float));
		bufferLayout.AddMember(memberC);

		BufferMember memberD("d", sizeof(Foo) + 4 * sizeof(float), 2 * 4 * sizeof(float));
		BufferMember subMemberFooD0("d0", sizeof(Foo) + 4 * sizeof(float), 4 * sizeof(float));
		BufferMember subMemberFooD1("d1", sizeof(Foo) + 2 * 4 * sizeof(float), 4 * sizeof(float));
		memberD.AddSubMember(subMemberFooD0);
		memberD.AddSubMember(subMemberFooD1);
		bufferLayout.AddMember(memberD);

		std::string result = bufferLayout.ToString();
		LOG_TRACE("\n{}", result);

		std::string expectation;
		expectation += "testLayout: size=64\n";
		expectation += "  foo: offset=0, size=16\n";
		expectation += "    a: offset=0, size=4\n";
		expectation += "    b: offset=4, size=4\n";
		expectation += "  c: offset=16, size=16\n";
		expectation += "  d: offset=32, size=32\n";
		expectation += "    d0: offset=32, size=16\n";
		expectation += "    d1: offset=48, size=16";
	
		EXPECT_TRUE(result == expectation);
	}
	catch (const std::exception& e)
	{
		FAIL() << "Exception thrown:\n" << e.what();
	}

	// For more manual tests use this webpage to create proper buffer layouts and try to recreate them here manually:
	// https://maraneshi.github.io/HLSL-ConstantBufferLayoutVisualizer/
	// e.g. the above bufferLayout can be constructed on the webpage with this pseudo code:
	// struct Foo
	// {
	//     int a;
	//     float b;
	// };
	// cbuffer example
	// {
	//     Foo foo;
	//     float4 c;
	//     float4 d[2];
	// };
}