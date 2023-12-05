#include <iostream>
#include <windows.h>
#include <thread>
#include "demofuncs" // Підключення власного заголовочного файлу з функціями


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
		std::cout << "process F broken " << std::endl; // Виведення повідомлення про непрацездатність процесу F

		return 1;
	}
	const DWORD processID = std::atoi(argv[1]);
	const int32_t x = std::atoi(argv[2]);
	
	// Тут викликається функція

	const double answerInDouble = spos::lab1::demo::f_func<spos::lab1::demo::DOUBLE>(x);
	std::cout << "calc = " << answerInDouble << std::endl; // Виведення результату розрахунку
	const std::pair<UINT, LPARAM> separateAnswer = makeMessageFromDouble(answerInDouble);
	PostThreadMessageW(processID, separateAnswer.first + WM_USER + 1, separateAnswer.second, 0);
	return 0;
}