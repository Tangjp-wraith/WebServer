/**
 * @file buffer_test.cpp
 * @author Tang Jiapeng (tangjiapeng0215@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../src/include/buffer.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using std::string;

TEST(buffer_test, test1) {
  Buffer buf;
  EXPECT_EQ(buf.ReadableBytes(), 0);
  EXPECT_EQ(buf.PrependableBytes(), 0);
  EXPECT_EQ(buf.WriteableBytes(), 1024);

  const string str(200, 'a');
  buf.Append(str);
  EXPECT_EQ(buf.ReadableBytes(), str.size());
  EXPECT_EQ(buf.PrependableBytes(), 0);
  EXPECT_EQ(buf.WriteableBytes(), 1024 - str.size());

  buf.Append(string(800, 'a'));
  buf.Retrieve(900);
  EXPECT_EQ(buf.ReadableBytes(), 100);
  EXPECT_EQ(buf.PrependableBytes(), 900);
  EXPECT_EQ(buf.WriteableBytes(), 24);

  buf.Append(string(200, 'c'));
  EXPECT_EQ(buf.ReadableBytes(), 300);
  EXPECT_EQ(buf.PrependableBytes(), 0);
  EXPECT_EQ(buf.WriteableBytes(), 1024 - 300);
  const string str1 = buf.RetrieveAllToStr();
  EXPECT_EQ(str1.size(), 300);
  EXPECT_EQ(str1, string(100, 'a') + string(200, 'c'));
  EXPECT_EQ(buf.ReadableBytes(), 0);
  EXPECT_EQ(buf.PrependableBytes(), 0);
  EXPECT_EQ(buf.WriteableBytes(), 1024);
}

TEST(buffer_test, test2) {
  Buffer buf;
  buf.Append(string(400, 'a'));
  EXPECT_EQ(buf.ReadableBytes(), 400);
  EXPECT_EQ(buf.PrependableBytes(), 0);
  EXPECT_EQ(buf.WriteableBytes(), 1024 - 400);

  buf.Retrieve(50);
  EXPECT_EQ(buf.ReadableBytes(), 350);
  EXPECT_EQ(buf.PrependableBytes(), 50);
  EXPECT_EQ(buf.WriteableBytes(), 1024 - 400);

  buf.Append(string(1000, 'c'));
  EXPECT_EQ(buf.ReadableBytes(), 1350);
  EXPECT_EQ(buf.PrependableBytes(), 50);
  EXPECT_EQ(buf.WriteableBytes(), 1);

  char *data = const_cast<char *>(buf.Peek());
  string str = data;
  EXPECT_EQ(str, string(350, 'a') + string(1000, 'c'));

  buf.RetrieveUntil(data + 100);
  EXPECT_EQ(buf.ReadableBytes(), 1250);
  EXPECT_EQ(buf.PrependableBytes(), 150);
  EXPECT_EQ(buf.WriteableBytes(), 1);

  char *data1 = const_cast<char *>(buf.BeginWriteConst());
  str = data1;
  EXPECT_EQ(str, "");
  str = data1 - 1;
  EXPECT_EQ(str, "c");

  buf.RetrieveAll();
  EXPECT_EQ(buf.ReadableBytes(), 0);
  EXPECT_EQ(buf.PrependableBytes(), 0);
  EXPECT_EQ(buf.WriteableBytes(), 1401);

  int x=0;
  buf.Append(&x,sizeof(x));
  EXPECT_EQ(buf.ReadableBytes(), 4);
  EXPECT_EQ(buf.PrependableBytes(), 0);
  EXPECT_EQ(buf.WriteableBytes(), 1401-4);
}