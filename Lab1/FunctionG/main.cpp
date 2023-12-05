#include <iostream>
#include <windows.h>
#include <thread>
#include "demofuncs" // ϳ��������� �������� ������������� ����� � ���������


std::pair<UINT, LPARAM> makeMessageFromDouble(double msg)
{
  union T
  {
    struct
    {
      UINT uint;
	  LPARAM lparam;
    } s;
    double d;
  } u;
  u.d = msg;

  return { u.s.uint, u.s.lparam };
}

int main(int argc, TCHAR* argv[])
{
	if (argc < 2)
	{
		std::cout << "process G broken " << std::endl; // ���� �����������, �� ������ G �� ������
		return 1;
	}
	const DWORD processID = std::atoi(argv[1]);
	const int32_t x = std::atoi(argv[2]);

	// ��� ����������� �������

	const double answerInDouble = spos::lab1::demo::g_func<spos::lab1::demo::DOUBLE>(x);
	std::cout << "calc = " << answerInDouble << std::endl; // ���� �������������� ����������
	const std::pair<UINT, LPARAM> separateAnswer = makeMessageFromDouble(answerInDouble);
	PostThreadMessageW(processID, separateAnswer.first + WM_USER + 1, separateAnswer.second, 1);
	return 0;
}