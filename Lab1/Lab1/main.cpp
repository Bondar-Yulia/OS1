#include <iostream>
#include <thread>
#include <windows.h>
#include <conio.h>
#include <tchar.h>
#include <chrono>
#include <future>
#include <sstream>

namespace MyNamespace
{
	enum class InputKey
	{
		Yes,
		No,
		Esc,
		None
	};

	/* глобальні змінні */

	uint32_t DefaultSize = 100;
	uint32_t ExitParam = 0;
	uint32_t SecondsToTryTerminate = 10;
	//bool bEscapeRequest = false; // флаг запиту на вихід
	bool bIsErrorMessageCreated = false;
	InputKey inputKey = InputKey::None;
	bool bRequestTimerStart = false;
	bool bIsTimerStart = false;
	bool bIsTimerDone = false;
	std::chrono::steady_clock::time_point startTimerPoint;

	/* методи */

	void cleanUpCin()
	{
		std::cout << "Incorrect input" << std::endl; // Виведення повідомлення про некоректний ввід
		std::cin.clear();
		while (std::cin.get() != '\n')
		{
			std::cin.ignore();
		}
	}

	bool CreateProcess(char* name, LPSTARTUPINFOA startupInfo, LPPROCESS_INFORMATION processInfo, DWORD flag = 0)
	{
		return CreateProcess(
			NULL,            // Без імені модуля (використовувати командний рядок)
			name,                          // Командний рядок
			NULL,         // Наслідування дескриптора процесу не дозволено
			NULL,         // Наслідування дескриптора потоку не дозволено
			FALSE,        // Встановлення спадкування дескриптора на FALSE
			flag,            // Без флагів створення
			NULL,          // Використання блоку середовища батьківського процесу
			NULL,     // Використання початкової директорії батьківського процесу
			startupInfo,             // Вказівник на структуру STARTUPINFO
			processInfo);           // Вказівник на структуру PROCESS_INFORMATION
	}

	double getDoubleFromMessage(std::pair<UINT, LPARAM> msg)
	{
		union T
		{
			struct
			{
				UINT ui;
				LPARAM lp;
			} s;
			double d;
		} u;
		u.s.ui = msg.first;
		u.s.lp = msg.second;
		return u.d;
	}


}


int main(int argc, TCHAR* argv[])
{
	auto f_proc = std::async(std::launch::async, []()
		{
			while (true)
			{
				// Обробка введення клавіш
				if (GetAsyncKeyState(VK_ESCAPE) == -32767)
					MyNamespace::inputKey = MyNamespace::InputKey::Esc;
				else if (GetAsyncKeyState(0x59) == -32767)
					MyNamespace::inputKey = MyNamespace::InputKey::Yes;
				else if (GetAsyncKeyState(0x4E) == -32767)
					MyNamespace::inputKey = MyNamespace::InputKey::No;
				//MyNamespace::inputKey = MyNamespace::InputKey::None;
			}
		});

	auto timer = std::async(std::launch::async, ([]()
		{
			while (true)
			{
				// Логіка таймера
				if (MyNamespace::bRequestTimerStart)
				{
					if (!MyNamespace::bIsTimerStart)
					{
						MyNamespace::startTimerPoint = std::chrono::high_resolution_clock::now();
						MyNamespace::bIsTimerStart = true;
					}
					else
					{
						if (std::chrono::duration_cast<std::chrono::seconds>((std::chrono::high_resolution_clock::now() - MyNamespace::startTimerPoint)).count() > MyNamespace::SecondsToTryTerminate)
						{
							MyNamespace::bIsTimerDone = true;
						}
					}
				}
				else
				{
					//MyNamespace::bRequestTimerStart = false;
					MyNamespace::bIsTimerStart = false;
					MyNamespace::bIsTimerDone = false;
				}
			}
		}));

	while (true)
	{
		// Підготовка до нової ітерації
		MyNamespace::bRequestTimerStart = false;
		MyNamespace::bIsTimerStart = false;
		MyNamespace::bIsTimerDone = false;
		MyNamespace::bIsErrorMessageCreated = false;

		system("cls");
		{
			int32_t x;
			std::cout << "Enter the x" << std::endl;
			while (!(std::cin >> x))
			{
				MyNamespace::cleanUpCin();
			}
			if (x == -1)
			{
				exit(0);
			}
			MyNamespace::inputKey = MyNamespace::InputKey::None;

			// Створення процесу G
			STARTUPINFO siG;
			PROCESS_INFORMATION piG;

			ZeroMemory(&siG, sizeof(siG));
			siG.cb = sizeof(siG);
			ZeroMemory(&piG, sizeof(piG));

			char* processName = new char[MyNamespace::DefaultSize];
			const DWORD thisThreadId = GetCurrentThreadId();
			//std::cout << "this thread = " << thisThreadId << std::endl;

			std::string stringName = "FunctionG.exe " + std::to_string(thisThreadId) + " " + std::to_string(x);
			strcpy_s(processName, MyNamespace::DefaultSize, stringName.c_str());

			if (!MyNamespace::CreateProcessA(processName, &siG, &piG))
			{
				std::cout << "CreateProcess G failed" << std::endl;
				return 1;
			}

			// Створення процесу F
			STARTUPINFO siF;
			PROCESS_INFORMATION piF;

			ZeroMemory(&siF, sizeof(siF));
			siF.cb = sizeof(siF);
			ZeroMemory(&piF, sizeof(piF));

			stringName = "FunctionF.exe " + std::to_string(thisThreadId) + " " + std::to_string(x);
			strcpy_s(processName, MyNamespace::DefaultSize, stringName.c_str());

			if (!MyNamespace::CreateProcessA(processName, &siF, &piF))
			{
				std::cout << "CreateProcess F failed" << std::endl;
				return 1;
			}
			MSG msg;
			//std::cout << "start to getting message..." << std::endl;

			// Обробка повідомлень від процесів
			STARTUPINFO siError;
			PROCESS_INFORMATION piError;
			DWORD errorExitCode = 0;
			MyNamespace::bRequestTimerStart = true;
		
			std::pair<bool, double> a{ false, 0 }, b{ false, 0 };
			while (!a.first || !b.first)
			{
				if (MyNamespace::inputKey == MyNamespace::InputKey::Esc || MyNamespace::bIsTimerDone)
				{
					MyNamespace::bRequestTimerStart = false;
					MyNamespace::bIsTimerStart = false;
					MyNamespace::bIsTimerDone = false;

					if (!MyNamespace::bIsErrorMessageCreated)
					{
						ZeroMemory(&siError, sizeof(siError));
						siError.cb = sizeof(siError);
						ZeroMemory(&piError, sizeof(piError));

						char errorProcessName[] = "ErrorMessage";
						if (!MyNamespace::CreateProcessA(errorProcessName, &siError, &piError, CREATE_NEW_CONSOLE))
						{
							std::cout << "Create ErrorProcess failed" << std::endl;
						}
						MyNamespace::bIsErrorMessageCreated = true;
					}
					MyNamespace::inputKey = MyNamespace::InputKey::None;
				}
				if (MyNamespace::bIsErrorMessageCreated)
				{
					GetExitCodeProcess(piError.hProcess, &errorExitCode);
					if (MyNamespace::inputKey == MyNamespace::InputKey::Yes || errorExitCode != STILL_ACTIVE)
					{
						errorExitCode != STILL_ACTIVE ? std::cout << "Calculation was stopped because time for answer has left" << std::endl : std::cout << "Calculation was stopped by User" << std::endl;
						MyNamespace::bIsErrorMessageCreated = false;
						MyNamespace::bRequestTimerStart = true;
						TerminateProcess(piF.hProcess, 0);
						TerminateProcess(piG.hProcess, 0);
						TerminateProcess(piError.hProcess, 0);
						CloseHandle(piError.hProcess);
						break;
					}
					else if (MyNamespace::inputKey == MyNamespace::InputKey::No)
					{
						MyNamespace::bRequestTimerStart = true;
						MyNamespace::bIsTimerStart = false;
						MyNamespace::bIsTimerDone = false;
						MyNamespace::bIsErrorMessageCreated = false;
						TerminateProcess(piError.hProcess, 0);
						CloseHandle(piError.hProcess);
					}
					MyNamespace::inputKey = MyNamespace::InputKey::None;
				}
				const BOOL bRet = PeekMessage(&msg, NULL, 0, UINT32_MAX, PM_REMOVE);
				//const BOOL bRet = GetMessage(&msg, NULL, 0,UINT32_MAX);
				if (bRet)
				{
					const double Message = MyNamespace::getDoubleFromMessage(std::pair<UINT, LPARAM>(msg.message - WM_USER - 1, msg.wParam));
					//std::cout << "Got new Message! " << Message << std::endl;
					if (!a.first && msg.lParam == 0)
					{
						a.first = true;
						a.second = Message;
					}
					if (!b.first && msg.lParam == 1)
					{
						b.first = true;
						b.second = Message;
					}
					if (Message == MyNamespace::ExitParam)
					{
						TerminateProcess(piF.hProcess, 0);
						TerminateProcess(piG.hProcess, 0);
						//std::cout << "Program end , because one of params is " << MyNamespace::ExitParam << std::endl;
						break;
					}
				}
			}

			if (MyNamespace::bIsErrorMessageCreated)
			{
				TerminateProcess(piError.hProcess, 0);
				CloseHandle(piError.hProcess);
			}
			CloseHandle(piF.hProcess);
			CloseHandle(piG.hProcess);
			//std::cout << "Program finished successfully!" << std::endl;
			//std::this_thread::sleep_for(std::chrono::seconds(2));
			if (!a.first && !b.first)
			{
				std::cout << "f and g wasn't calculated" << std::endl;
			}
			else if (a.first && b.first)
			{
				std::cout << "f = " << a.second << " and g = " << b.second << std::endl;
			}
			else
			{
				if (!a.first)
				{
					std::cout << "g = " << b.second <<" and f wasn't calculated" << std::endl;
				}
				if (!b.first)
				{
					std::cout << "f = " << a.second << " and g wasn't calculated" << std::endl;
				}
			}
			//std::cout << "Output is " << (a.first ? std::to_string(a.second) : "f wasn't calculated") << " and " << (b.first ? std::to_string(b.second) : "g wasn't calculated") << std::endl;
			std::cout << "Enter smth to continue..." << std::endl;
			std::cin.get();
			std::cin.get();
			//_getch();
		}
	}
	return 0;
}
