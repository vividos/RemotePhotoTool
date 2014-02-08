//
// ulib - a collection of useful classes
// Copyright (C) 2013 Michael Fink
//
/// \file TestString.cpp tests CString class
//

// includes
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BaseUnitTest
{
TEST_CLASS(TestString)
{
public:
   /// tests default ctor
   TEST_METHOD(TestDefaultCtor)
   {
      CString s1, s2;
      Assert::AreEqual(s1.GetLength(), s2.GetLength());
   }

   /// tests ctors
   TEST_METHOD(TestCtors)
   {
      CString s1(_T("abc123"));
      CString s2("abc123");
      CString s3(L"abc123");
      Assert::AreEqual(s1.GetString(), s2.GetString());
      Assert::AreEqual(s1.GetString(), s3.GetString());

      CString s4(_T('a'), 3);
      CString s5(_T("aaabbb"), 3);
      Assert::AreEqual(s4.GetString(), s5.GetString());
      Assert::AreEqual(s4.GetString(), _T("aaa"));

      CString s6(_T('a'));
      Assert::AreEqual(s6.GetString(), _T("a"));

      CString s7(_T(""));
      LPCTSTR psz8 = nullptr;
      CString s8(psz8);
      Assert::AreEqual(s7.GetString(), s8.GetString());
   }

   /// tests converting ctors
   TEST_METHOD(TestConvertingCtors1)
   {
      CString s1(_T("abc123"));

      CStringA s2(s1);
      Assert::AreEqual(s2.GetString(), "abc123");

      CStringW s3(s1);
      Assert::AreEqual(s3.GetString(), L"abc123");
   }

   /// tests ctors that take the "other" char type as arguments
   TEST_METHOD(TestConvertingCtors2)
   {
      CStringA s1(L"abc123");
      Assert::AreEqual(s1.GetString(), "abc123");

      CStringW s2("abc123");
      Assert::AreEqual(s2.GetString(), L"abc123");
   }

   /// tests copy ctor
   TEST_METHOD(TestCopyCtor)
   {
      CString s1(_T("abc123"));
      CString s2(s1);

      Assert::AreEqual(s2.GetString(), _T("abc123"));
   }

   /// tests GetLength()
   TEST_METHOD(TestGetLength)
   {
      CString s1(_T("abc123"));
      CString s2(_T('y'), 555);

      Assert::AreEqual(s1.GetLength(), 6);
      Assert::AreEqual(s2.GetLength(), 555);
   }

   /// tests IsEmpty()
   TEST_METHOD(TestIsEmpty)
   {
      CString s1(_T("abc123"));
      CString s2(_T(""));
      CString s3;

      Assert::IsFalse(s1.IsEmpty());
      Assert::IsTrue(s2.IsEmpty());
      Assert::IsTrue(s3.IsEmpty());
   }

   /// tests Empty()
   TEST_METHOD(TestEmpty)
   {
      CString s1(_T("abc123"));
      CString s2(_T(""));

      Assert::IsFalse(s1.IsEmpty());
      Assert::IsTrue(s2.IsEmpty());

      s1.Empty();
      s2.Empty();

      Assert::IsTrue(s1.IsEmpty());
      Assert::IsTrue(s2.IsEmpty());
   }

   /// tests GetAt()
   TEST_METHOD(TestGetAt)
   {
      CString s1(_T("abc123"));

      Assert::AreEqual(s1.GetAt(1), _T('b'));
   }

   /// tests operator[]
   TEST_METHOD(TestIndexOperator)
   {
      CString s1(_T("abc123"));

      Assert::AreEqual(s1[2], _T('c'));
   }

   /// tests SetAt()
   TEST_METHOD(TestSetAt)
   {
      CString s1(_T("abc123"));

      s1.SetAt(2, _T('_'));
      Assert::AreEqual(s1.GetAt(2), _T('_'));
   }

   /// tests operator LPCTSTR
   TEST_METHOD(TestOperatorLPCTSTR)
   {
      CString s1(_T("abc123"));

      LPCTSTR psz1 = s1;

      Assert::AreEqual(psz1, _T("abc123"));
   }

   /// tests operator=()
   TEST_METHOD(TestAssignmentOperator)
   {
      CString s1(_T("abc123"));
      CString s2;

      s2 = s1;

      Assert::AreEqual(s2.GetString(), _T("abc123"));

      s2 = s2;
      Assert::AreEqual(s2.GetString(), _T("abc123"));
   }

   /// tests operator+()
   TEST_METHOD(TestAddOperator)
   {
      CString s1(_T("abc"));
      CString s2(_T("123"));

      CString s3 = s1 + s2;
      Assert::AreEqual(s3.GetString(), _T("abc123"));

      s3 = s3 + s3;
      Assert::AreEqual(s3.GetString(), _T("abc123abc123"));
   }

   /// tests operator+=()
   TEST_METHOD(TestInplaceAddOperator)
   {
      CString s1(_T("abc"));
      s1 += _T("123");

      Assert::AreEqual(s1.GetString(), _T("abc123"));

      s1 += s1;
      Assert::AreEqual(s1.GetString(), _T("abc123abc123"));
   }

   /// tests operator==()
   TEST_METHOD(TestEqualityOperator)
   {
      CString s1(_T("abc123"));
      CString s2(s1);
      CString s3(_T("123abc"));

      Assert::IsTrue(s1 == s1);
      Assert::IsTrue(s1 == s2);
      Assert::IsFalse(s1 == s3);

      Assert::IsTrue(s1 == s1.GetString());
      Assert::IsTrue(s1 == s2.GetString());
      Assert::IsFalse(s1 == s3.GetString());

      Assert::IsTrue(s1.GetString() == s1);
      Assert::IsTrue(s1.GetString() == s2);
      Assert::IsFalse(s1.GetString() == s3);
   }

   /// tests operator!=()
   TEST_METHOD(TestInequalityOperator)
   {
      CString s1(_T("abc123"));
      CString s2(s1);
      CString s3(_T("123abc"));

      Assert::IsFalse(s1 != s1);
      Assert::IsFalse(s1 != s2);
      Assert::IsTrue(s1 != s3);

      Assert::IsFalse(s1 != s1.GetString());
      Assert::IsFalse(s1 != s2.GetString());
      Assert::IsTrue(s1 != s3.GetString());

      Assert::IsFalse(s1.GetString() != s1);
      Assert::IsFalse(s1.GetString() != s2);
      Assert::IsTrue(s1.GetString() != s3);
   }

   /// tests operator<()
   TEST_METHOD(TestLessOperator)
   {
      CString s1(_T("abc"));
      CString s2(_T("def"));
      CString s3(s1);

      Assert::IsTrue(s1 < s2);
      Assert::IsFalse(s2 < s1);
      Assert::IsFalse(s1 < s3); // equal cases
      Assert::IsFalse(s3 < s1);

      Assert::IsTrue(s1 < s2.GetString());

      Assert::IsTrue(s1.GetString() < s2);
   }

   /// tests operator>()
   TEST_METHOD(TestGreaterOperator)
   {
      CString s1(_T("abc"));
      CString s2(_T("def"));
      CString s3(s1);

      Assert::IsTrue(s2 > s1);
      Assert::IsFalse(s1 > s2);
      Assert::IsFalse(s1 > s3); // equal cases
      Assert::IsFalse(s3 > s1);

      Assert::IsTrue(s2 > s1.GetString());

      Assert::IsTrue(s2.GetString() > s1);
   }

   /// tests operator<=()
   TEST_METHOD(TestLessOrEqualOperator)
   {
      CString s1(_T("abc"));
      CString s2(_T("def"));
      CString s3(s1);

      Assert::IsTrue(s1 <= s2);
      Assert::IsFalse(s2 <= s1);
      Assert::IsTrue(s1 <= s3); // equal cases
      Assert::IsTrue(s3 <= s1);

      Assert::IsTrue(s1 <= s2.GetString());

      Assert::IsTrue(s1.GetString() <= s2);
   }

   /// tests operator>=()
   TEST_METHOD(TestGreaterOrEqualOperator)
   {
      CString s1(_T("abc"));
      CString s2(_T("def"));
      CString s3(s1);

      Assert::IsTrue(s2 >= s1);
      Assert::IsFalse(s1 >= s2);
      Assert::IsTrue(s1 >= s3); // equal cases
      Assert::IsTrue(s3 >= s1);

      Assert::IsTrue(s2 >= s1.GetString());

      Assert::IsTrue(s2.GetString() >= s1);
   }

   /// tests Compare()
   TEST_METHOD(TestCompare)
   {
      CString s1(_T("abc"));
      CString s2(_T("def"));
      CString s3(s1);

      Assert::IsTrue(s1.Compare(s2) < 0);
      Assert::IsTrue(s1.Compare(s3) == 0);
      Assert::IsTrue(s2.Compare(s1) > 0);
   }

   /// tests CompareNoCase()
   TEST_METHOD(TestCompareNoCase)
   {
      CString s1(_T("abc"));
      CString s2(_T("DEF"));
      CString s3(s1);

      Assert::IsFalse(s1.Compare(s2) < 0); // strings in wrong order due to case

      Assert::IsTrue(s1.CompareNoCase(s2) < 0);
      Assert::IsTrue(s1.CompareNoCase(s3) == 0);
      Assert::IsTrue(s2.CompareNoCase(s1) > 0);
   }

   /// tests Mid()
   TEST_METHOD(TestMid)
   {
      CString s1(_T("abc123"));

      CString s2 = s1.Mid(3);
      CString s3 = s1.Mid(2,3);
      CString s4 = s1.Mid(4,3);
      CString s5 = s1.Mid(8,3);
      CString s6 = s1.Mid(2,0);

      Assert::AreEqual(s2.GetString(), _T("123"));
      Assert::AreEqual(s3.GetString(), _T("c12"));
      Assert::AreEqual(s4.GetString(), _T("23"));
      Assert::IsTrue(s5.IsEmpty());
      Assert::IsTrue(s6.IsEmpty());
   }

   /// tests Left()
   TEST_METHOD(TestLeft)
   {
      CString s1(_T("abc123"));

      CString s2 = s1.Left(3);
      CString s3 = s1.Left(0);
      CString s4 = s1.Left(7);

      Assert::AreEqual(s2.GetString(), _T("abc"));
      Assert::IsTrue(s3.IsEmpty());
      Assert::AreEqual(s4.GetString(), _T("abc123"));
   }

   /// tests Right()
   TEST_METHOD(TestRight)
   {
      CString s1(_T("abc123"));

      CString s2 = s1.Right(3);
      CString s3 = s1.Right(0);
      CString s4 = s1.Right(7);

      Assert::AreEqual(s2.GetString(), _T("123"));
      Assert::IsTrue(s3.IsEmpty());
      Assert::AreEqual(s4.GetString(), _T("abc123"));
   }

   /// tests SpanIncluding()
   TEST_METHOD(TestSpanIncluding)
   {
      CString s1(_T("cabbage"));

      CString s2 = s1.SpanIncluding(_T("abc"));
      CString s3 = s1.SpanIncluding(_T("xyz"));

      Assert::AreEqual(s2.GetString(), _T("cabba"));
      Assert::IsTrue(s3.IsEmpty());
   }

   /// tests SpanExcluding()
   TEST_METHOD(TestSpanExcluding)
   {
      CString s1(_T("cabbage"));
      CString s2 = s1.SpanExcluding(_T("abc"));
      CString s3 = s1.SpanExcluding(_T("xyz"));

      //Assert::AreEqual(s2.GetString(), _T("ge")); // TODO wrong?
      Assert::AreEqual(s3.GetString(), _T("cabbage"));
   }

   /// tests MakeUpper()
   TEST_METHOD(TestMakeUpper)
   {
      CString s1(_T("abc123"));
      CString s2(_T("aouäöüß123"));

      s1.MakeUpper();
      s2.MakeUpper();

      Assert::AreEqual(s1.GetString(), _T("ABC123"));
      //Assert::AreEqual(s2.GetString(), _T("AOUÄÖÜß123"));
   }

   /// tests MakeLower()
   TEST_METHOD(TestMakeLower)
   {
      CString s1(_T("ABC123"));
      CString s2(_T("AOUÄÖÜß123"));

      s1.MakeLower();
      s2.MakeLower();

      Assert::AreEqual(s1.GetString(), _T("abc123"));
      //Assert::AreEqual(s2.GetString(), _T("aouäöüß123"));
   }

   /// tests MakeReverse()
   TEST_METHOD(TestMakeReverse)
   {
      CString s1(_T("abc123"));

      s1.MakeReverse();

      Assert::AreEqual(s1.GetString(), _T("321cba"));
   }

   /// tests Replace()
   TEST_METHOD(TestReplace)
   {
      CString s1(_T("abc123"));
      CString s2(_T("abc123abc456"));
      CString s3(_T("aabbabacda"));

      int ret = s1.Replace(_T("bc"), _T("xy"));
      Assert::AreEqual(ret, 1);
      Assert::AreEqual(s1.GetString(), _T("axy123"));

      ret = s2.Replace(_T("bc"), _T("xy"));
      Assert::AreEqual(ret, 2);
      Assert::AreEqual(s2.GetString(), _T("axy123axy456"));

      ret = s3.Replace(_T('a'), _T('x'));
      ret += s3.Replace(_T('b'), _T('y'));
      Assert::AreEqual(ret, 8);
      Assert::AreEqual(s3.GetString(), _T("xxyyxyxcdx"));
   }

   /// tests Remove()
   TEST_METHOD(TestRemove)
   {
      CString s1(_T("abc123abc456"));
      CString s2(_T("aabbabacda"));

      int ret = s1.Remove(_T('b'));
      Assert::AreEqual(ret, 2);
      Assert::AreEqual(s1.GetString(), _T("ac123ac456"));

      ret = s2.Remove(_T('a'));
      ret += s2.Remove(_T('b'));
      Assert::AreEqual(ret, 8);
      Assert::AreEqual(s2.GetString(), _T("cd"));
   }

   /// tests Insert() with strings
   TEST_METHOD(TestInsertStr)
   {
      CString s1(_T("abc123"));
      CString s2(_T("abc123"));
      CString s3(_T("abc123"));

      int ret = s1.Insert(3, _T("xyz"));
      Assert::AreEqual(ret, 9);
      Assert::AreEqual(s1.GetString(), _T("abcxyz123"));

      ret = s2.Insert(6, _T("xyz"));
      Assert::AreEqual(ret, 9);
      Assert::AreEqual(s2.GetString(), _T("abc123xyz"));

      ret = s3.Insert(8, _T("xyz"));
      Assert::AreEqual(ret, 9);
      Assert::AreEqual(s3.GetString(), _T("abc123xyz"));
   }

   /// tests Insert() with chars
   TEST_METHOD(TestInsertCh)
   {
      CString s1(_T("abc123"));
      CString s2(_T("abc123"));
      CString s3(_T("abc123"));

      int ret = s1.Insert(3, _T('-'));
      Assert::AreEqual(ret, 7);
      Assert::AreEqual(s1.GetString(), _T("abc-123"));

      ret = s2.Insert(6, _T('x'));
      Assert::AreEqual(ret, 7);
      Assert::AreEqual(s2.GetString(), _T("abc123x"));

      ret = s3.Insert(8, _T('x'));
      Assert::AreEqual(ret, 7);
      Assert::AreEqual(s3.GetString(), _T("abc123x"));
   }

   /// tests Delete()
   TEST_METHOD(TestDelete)
   {
      CString s1(_T("abc123"));
      CString s2(_T("abc123"));
      CString s3(_T("abc123"));

      int ret = s1.Delete(3);
      Assert::AreEqual(ret, 5);
      Assert::AreEqual(s1.GetString(), _T("abc23"));

      ret = s2.Delete(5, 3);
      Assert::AreEqual(ret, 5);
      Assert::AreEqual(s2.GetString(), _T("abc12"));

      ret = s3.Delete(7, 2);
      Assert::AreEqual(ret, 6);
      Assert::AreEqual(s3.GetString(), _T("abc123"));
   }

   /// tests Format()
   TEST_METHOD(TestFormat)
   {
      CString s1;

      s1.Format(_T("%s %i %.3u % 3.4f"),
         _T("abc"), -1, 42, 64.42);

      Assert::AreEqual(s1.GetString(), _T("abc -1 042  64.4200"));
   }

   static void FormatVHelper(CString& s, LPCTSTR pszText, ...)
   {
      va_list args;
      va_start(args, pszText);

      s.FormatV(pszText, args);

      va_end(args);
   }

   /// tests FormatV()
   TEST_METHOD(TestFormatV)
   {
      CString s1;

      FormatVHelper(s1, _T("%s %i %.3u % 3.4f"),
         _T("abc"), -1, 42, 64.42);

      Assert::AreEqual(s1.GetString(), _T("abc -1 042  64.4200"));
   }

   /// tests AppendFormat()
   TEST_METHOD(TestAppendFormat)
   {
      CString s1(_T("abc123"));

      s1.AppendFormat(_T("%s %i %.3u % 3.4f"),
         _T("abc"), -1, 42, 64.42);

      Assert::AreEqual(s1.GetString(), _T("abc123abc -1 042  64.4200"));
   }

   static void AppendFormatVHelper(CString& s, LPCTSTR pszText, ...)
   {
      va_list args;
      va_start(args, pszText);

      s.AppendFormatV(pszText, args);

      va_end(args);
   }

   /// tests AppendFormatV()
   TEST_METHOD(TestAppendFormatV)
   {
      CString s1(_T("abc123"));

      AppendFormatVHelper(s1, _T("%s %i %.3u % 3.4f"),
         _T("abc"), -1, 42, 64.42);

      Assert::AreEqual(s1.GetString(), _T("abc123abc -1 042  64.4200"));
   }
   /// tests TrimLeft()
   TEST_METHOD(TestTrimLeft)
   {
      CString s1(_T("  \t\nabc123 \n\t"));
      CString s2(_T("abc123"));
      CString s3(_T("aaabc123333"));

      s1.TrimLeft();
      Assert::AreEqual(s1.GetString(), _T("abc123 \n\t"));

      s2.TrimLeft(_T("abc"));
      Assert::AreEqual(s2.GetString(), _T("123"));

      s3.TrimLeft(_T('a'));
      Assert::AreEqual(s3.GetString(), _T("bc123333"));
   }

   /// tests TrimRight()
   TEST_METHOD(TestTrimRight)
   {
      CString s1(_T("  \t\nabc123 \n\t"));
      CString s2(_T("abc123"));
      CString s3(_T("aaabc123333"));

      s1.TrimRight();
      Assert::AreEqual(s1.GetString(), _T("  \t\nabc123"));

      s2.TrimRight(_T("123"));
      Assert::AreEqual(s2.GetString(), _T("abc"));

      s3.TrimRight(_T('3'));
      Assert::AreEqual(s3.GetString(), _T("aaabc12"));
   }

   /// tests Trim()
   TEST_METHOD(TestTrim)
   {
      CString s1(_T("  \t\nabc123 \n\t"));
      CString s2(_T("abc123"));
      CString s3(_T("aaabc123333"));

      s1.Trim();
      Assert::AreEqual(s1.GetString(), _T("abc123"));

      s2.Trim(_T("ab23"));
      Assert::AreEqual(s2.GetString(), _T("c1"));

      s3.Trim(_T('3'));
      Assert::AreEqual(s3.GetString(), _T("aaabc12"));
   }

   /// tests Find()
   TEST_METHOD(TestFind)
   {
      CString s1(_T("abc123"));
      CString s2(_T("abc123abc"));

      Assert::AreEqual(2, s1.Find(_T("c1")));
      Assert::AreEqual(-1, s1.Find(_T("c1"), 3));

      Assert::AreEqual(3, s1.Find(_T('1')));
      Assert::AreEqual(-1, s1.Find(_T('1'), 4));

      Assert::AreEqual(6, s2.Find(_T("a"), 2));
      Assert::AreEqual(6, s2.Find(_T('a'), 2));
   }

   /// tests ReverseFind()
   TEST_METHOD(TestReverseFind)
   {
      CString s1(_T("123abc123"));

      Assert::AreEqual(6, s1.ReverseFind(_T('1')));
      Assert::AreEqual(-1, s1.ReverseFind(_T('x')));
   }

   /// tests FindOneOf()
   TEST_METHOD(TestFindOneOf)
   {
      CString s1(_T("abc123"));

      Assert::AreEqual(3, s1.FindOneOf(_T("dx1")));
      Assert::AreEqual(2, s1.FindOneOf(_T("3cx")));
      Assert::AreEqual(-1, s1.FindOneOf(_T("xyz")));
   }

   /// tests GetBuffer(), ReleaseBuffer()
   TEST_METHOD(TestGetBufferReleaseBuffer)
   {
      CString s1(_T("abc123"));

      LPTSTR psz1 = s1.GetBuffer(7);
      Assert::AreEqual(_T("abc123"), psz1);

      psz1[2] = _T('x');

      s1.ReleaseBuffer();
      Assert::AreEqual(s1.GetString(), _T("abx123"));
   }

   /// tests GetBuffer(), ReleaseBufferSetLength()
   TEST_METHOD(TestGetBufferReleaseBufferSetLength)
   {
      CString s1(_T("abc123"));

      LPTSTR psz1 = s1.GetBuffer();
      Assert::AreEqual(_T("abc123"), psz1);

      psz1[2] = _T('y');

      s1.ReleaseBufferSetLength(4);
      Assert::AreEqual(s1.GetString(), _T("aby1"));
   }

   // missing tests from CStringT: Tokenize
   // missing tests from CSimpleStringT: GetBufferSetLength, FreeExtra, LockBuffer, UnlockBuffer

}; // class TestString

} // namespace BaseUnitTest
