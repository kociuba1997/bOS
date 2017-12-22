#include "../Interpreter/Interpreter.h"


Interpreter::Interpreter() {
	_PID = 0;
	_RegA = 0;
	_RegB = 0;
	_RegC = 0;
	_IP = 0;
}



void Interpreter::loadRegister() { // Running to wskaznik na PCB
	_PID = running->ProcessID;
	_RegA = running->A;
	_RegB = running->B;
	_RegC = running->C;
	_IP = running->CommandCounter;
}

std::string Interpreter::loadInstruction(Memory& RAM) {
	// wywolanie pamieci operacyjnej
	// zwraca nam rozkaz jaki ma zostac wykonany
	// return STRING Z ROZKAZEM
	return RAM.getCommand(_PID,_IP);
}

void Interpreter::setInstruction(Memory& RAM, int num_parameters)
{
	std::string command = "";
	instruction[0] = "";
	instruction[1] = "";
	instruction[2] = "";
	command = loadInstruction(RAM);
	instruction[0] = command.substr(0, 2);
	command = command.substr(2);
	if (num_parameters == 1) {
		int i = 1;
		while (command[i] != ' ') {
			instruction[1] += command[i];
			i++;
		}
	}
	if (num_parameters == 2) {
		int i = 1;
		while (command[i] != ' ') {
			instruction[1] += command[i];
			i++;
		}
			i++;
			instruction[2] = command.substr(i);
	}
}

void Interpreter::saveRegisters() {
	running->SetCommandCounter(_IP);
	running->A = _RegA;
	running->B = _RegB;
	running->C = _RegC;
	running->Timmer++;
}

void Interpreter::showRegisters() {
	std::cout << std::endl << "Stan reejestrow dla procesu: " << _PID << std::endl;
	std::cout << "A: " << _RegA << std::endl;
	std::cout << "B: " << _RegB << std::endl;
	std::cout << "C: " << _RegC << std::endl;
	std::cout << "Licznik rozkazow: " << _IP << std::endl;
}

void Interpreter::runInstruction(Disc& dysk, Memory& RAM, Scheduler& scheduler, Kolejka komunikacja) {
	_done = true; // ustalamy, �e rozkaz si� wykona. Jak nast�pi� probelmy to _done = false;
	loadRegister();
	// Rozkaz do wykonania
	std::string operation = loadInstruction(RAM).substr(0, 2);

	/* Operacje logiczne */
	// MV nazwa_rejestru liczba
	if (operation == ("MV")) {
		setInstruction(RAM, 2);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		if (d1 == "A") {
			if (d2 == "B") {
				_RegA = _RegB;
			}
			else if (d2 == "C") {
				_RegA = _RegC;
			}
			else if (d2 == "A") {
				_RegA = _RegA;
			}
			else {
				_RegA = std::stoi(d2);
			}
		}
		else if (d1 == "B") {
			if (d2 == "A") {
				_RegB = _RegA;
			}
			else if (d2 == "C") {
				_RegB = _RegC;
			}
			else if (d2 == "B") {
				_RegB = _RegB;
			}
			else {
				_RegB = std::stoi(d2);
			}
		}
		else if (d1 == "C") {
			if (d2 == "A") {
				_RegC = _RegA;
			}
			else if (d2 == "B") {
				_RegC = _RegB;
			}
			else if (d2 == "C") {
				_RegC = _RegC;
			}
			else {
				_RegC = std::stoi(d2);
			}
		}
	}

	// EQ nazwa_rejestru nazwa_rejestru
	else if (operation == ("EQ")) {
		setInstruction(RAM, 2);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		if (d1 == d2 || d1 == "A" && d2 == "B" && _RegA == _RegB ||
			d1 == "A" && d2 == "C" && _RegA == _RegC ||
			d1 == "B" && d2 == "A" && _RegB == _RegA ||
			d1 == "B" && d2 == "C" && _RegB == _RegC ||
			d1 == "C" && d2 == "A" && _RegC == _RegA ||
			d1 == "C" && d2 == "B" && _RegC == _RegB) {
			std::cout << "Wartosci sa sobie rowne!" << std::endl;
			_flagEQ = true;
		}
		else {
			_flagEQ = false;
			std::cout << "Wartosci sa rozne!" << std::endl;
		}
	}
	// JP gdzie
	else if (operation == ("JP")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		_IP = std::stoi(d1);
	}
	// JT gdzie(jesli prawda)
	else if (operation == ("JT")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		if (_flagEQ == true) _IP = std::stoi(d1);
	}
	// JF gdzie(je�li falsz)
	else if (operation == ("JF")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		if (_flagEQ == false) {
			_IP = std::stoi(d1);
			_IP--;
		}
	}

	/* Operacje arytmetyczne */
	// AD nazwa_rejestru liczba/nazwa_rejestru
	else if (operation == ("AD")) {
		setInstruction(RAM, 2);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		if (d1 == "A") {
			if (d2 == "B") {
				_RegA += _RegB;
			}
			else if (d2 == "C") {
				_RegA += _RegC;
			}
			else if (d2 == "A") {
				_RegA += _RegA;
			}
			else {
				_RegA += std::stoi(d2);
			}
		}
		else if (d1 == "B") {
			if (d2 == "A") {
				_RegB += _RegA;
			}
			else if (d2 == "C") {
				_RegB += _RegC;
			}
			else if (d2 == "B") {
				_RegB += _RegB;
			}
			else {
				_RegB += std::stoi(d2);
			}
		}
		else if (d1 == "C") {
			if (d2 == "A") {
				_RegC += _RegA;
			}
			else if (d2 == "B") {
				_RegC += _RegB;
			}
			else if (d2 == "C") {
				_RegC += _RegC;
			}
			else {
				_RegC += std::stoi(d2);
			}
		}
	}

	// ML nazwa_rejestru liczba/nazwa_rejestru
	else if (operation == ("ML")) {
		setInstruction(RAM, 2);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		if (d1 == "A") {
			if (d2 == "B") {
				_RegA *= _RegB;
			}
			else if (d2 == "C") {
				_RegA *= _RegC;
			}
			else if (d2 == "A") {
				_RegA *= _RegA;
			}
			else {
				_RegA *= std::stoi(d2);
			}
		}
		else if (d1 == "B") {
			if (d2 == "A") {
				_RegB *= _RegA;
			}
			else if (d2 == "C") {
				_RegB *= _RegC;
			}
			else if (d2 == "B") {
				_RegB *= _RegB;
			}
			else {
				_RegB *= std::stoi(d2);
			}
		}
		else if (d1 == "C") {
			if (d2 == "A") {
				_RegC *= _RegA;
			}
			else if (d2 == "B") {
				_RegC *= _RegB;
			}
			else if (d2 == "C") {
				_RegC *= _RegC;
			}
			else {
				_RegC *= std::stoi(d2);
			}
		}
	}

	// SB nazwa_rejestru liczba/nazwa_rejestru
	else if (operation == ("SB")) {
		setInstruction(RAM, 2);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		if (d1 == "A") {
			if (d2 == "B") {
				_RegA -= _RegB;
			}
			else if (d2 == "C") {
				_RegA -= _RegC;
			}
			else if (d2 == "A") {
				_RegA -= _RegA;
			}
			else {
				_RegA -= std::stoi(d2);
			}
		}
		else if (d1 == "B") {
			if (d2 == "A") {
				_RegB -= _RegA;
			}
			else if (d2 == "C") {
				_RegB -= _RegC;
			}
			else if (d2 == "B") {
				_RegB -= _RegB;
			}
			else {
				_RegB -= std::stoi(d2);
			}
		}
		else if (d1 == "C") {
			if (d2 == "A") {
				_RegC -= _RegA;
			}
			else if (d2 == "B") {
				_RegC -= _RegB;
			}
			else if (d2 == "C") {
				_RegC -= _RegC;
			}
			else {
				_RegC -= std::stoi(d2);
			}
		}
	}

	// DV nazwa_rejestru liczba/nazwa_rejestru
	else if (operation == ("DV")) {
		setInstruction(RAM, 2);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		if (d1 == "A") {
			if (d2 == "A") {
				_RegA /= _RegA;
			}
			else if (d2 == "B") {
				_RegA /= _RegB;
			}
			else if (d2 == "C") {
				_RegA /= _RegC;
			}
			else {
				_RegA /= std::stoi(d2);
			}
		}
		else if (d1 == "B") {
			if (d2 == "A") {
				_RegB /= _RegA;
			}
			else if (d2 == "B") {
				_RegB /= _RegB;
			}
			else if (d2 == "C") {
				_RegB /= _RegC;
			}
			else {
				_RegB /= std::stoi(d2);
			}
		}
		else if (d1 == "C") {
			if (d2 == "A") {
				_RegC /= _RegA;
			}
			else if (d2 == "B") {
				_RegC /= _RegB;
			}
			else if (d2 == "C") {
				_RegC /= _RegC;
			}
			else {
				if (std::stoi(d2) == 0) {
					std::cout << "ERROR! Nie mozna dzielic przez 0\n";
				}
				else _RegC /= std::stoi(d2);
			}
		}
	}

	// IC nazwa_rejestru
	else if (operation == ("IC")) {
		setInstruction(RAM, 1);
		std::string reg = instruction[1];
		if (reg == "A") {
			_RegA++;
		}
		else if (reg == "B") {
			_RegA++;
		}
		else if (reg == "C") {
			_RegA++;
		}
	}
	// DC nazwa_rejestru
	else if (operation == ("DC")) {
		setInstruction(RAM, 1);
		std::string reg = instruction[1];
		if (reg == "A") {
			_RegA--;
		}
		else if (reg == "B") {
			_RegB--;
		}
		else if (reg == "C") {
			_RegC--;
		}
	}

	/* Operacje wykonywane na dysku */
	// OF nazwa_pliku
	else if (operation == ("OF")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		if (!dysk.open_file(d1, false)) {
			_done = false;
		}
	}
	// ZF nazwa_pliku
	else if (operation == ("ZF")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		dysk.close_file(d1, false);
	}
	// CF nazwa_pliku
	else if (operation == ("CF")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		//dysk.create_file(d1);
		dysk.create_file(d1);
	}
	// WF nazwa_pliku dane
	else if (operation == ("WF")) {
		setInstruction(RAM, 2);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		dysk.write_file(d1, d2);
	}
	// PF nazwa_pliku
	else if (operation == ("PF")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		dysk.print_file(d1);
	}
	// DF nazwa_pliku
	else if (operation == ("DF")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		dysk.delete_file(d1);
	}
	// RF nazwa_pliku_stara nazwa_pliku_nowa
	else if (operation == ("RF")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		dysk.rename_file(d1, d2);
	}
	// AF nazwa_pliku dane
	else if (operation == ("AF")) {
		setInstruction(RAM, 2);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		if (d2 == "[A]") {
			d2 = std::to_string(_RegA);
		}
		else if (d2 == "[B]") {
			d2 = std::to_string(_RegB);
		}
		else if (d2 == "[C]") {
			d2 = std::to_string(_RegC);
		}
		dysk.add_to_file(d1, d2);
	}
	// LF
	else if (operation == ("LF")) {
		setInstruction(RAM, 0);
		dysk.print_file_list();
	}
	// PD
	else if (operation == ("PD")) {
		setInstruction(RAM, 0);
		dysk.printDisc();
	}




	/* Operacje wykonywane na komunikatach */
	// SC PID komunikat, gdzie ID_procesu to do kogo
	else if (operation == ("SC")) {
		setInstruction(RAM, 2);
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		Komunikat kom(_PID, d2);
		komunikacja.send(std::stoi(d1), kom);
	}
	// RC PID
	else if (operation == ("RC")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		Komunikat kom;
		kom = komunikacja.receive(std::stoi(d1));
	}
	// PC
	else if (operation == ("PC")) {
		setInstruction(RAM, 0);
		komunikacja.wyswietl();
	}

	/* Operacje wykonywane na procesach */
	// CP nazwa_procesu grupa_procesu
	else if (operation == ("CP")) {
		setInstruction(RAM, 2);
		//create a new process
		std::string d1 = instruction[1];
		std::string d2 = instruction[2];
		NewProcess(d1, std::stoi(d2));
		scheduler.Schedule();
	}
	// DP PID
	else if (operation == ("DP")) {
		setInstruction(RAM, 1);
		std::string d1 = instruction[1];
		DeleteProcess(std::stoi(d1));

	}

	// Aktywny proces: AP
	else if (operation == ("AP")) {
		setInstruction(RAM, 0);
		scheduler.print_running();
	}
	// Gotowe procesy: RP
	else if (operation == ("RP")) {
		setInstruction(RAM, 0);
		scheduler.wyswietl_gotowe();
	}
	/* Operacje wykonywane na pamieci RAM */
	// SR
	else if (operation == ("SR")) {
		setInstruction(RAM, 0);
		RAM.show();
	}
	// SF
	else if (operation == ("SF")) {
		setInstruction(RAM, 0);
		RAM.showFIFO();
	}
	// operator zamkniecia procesu
	else if (operation == ("EX")) {
		setInstruction(RAM, 0);
		// killujemy proces
		DeleteProcess(_PID);
		running->SetState(State::ZAKONCZONY);
	}
	// Blad interpretacji
	else {
		std::cout << "Brak takiej instrukcji";
		_done = false;
	}
	if (_done) {
		_IP++;
		saveRegisters();
	}
}


/*
ROZKAZY -> deasmbleracja
D1 - pierwsza dana
D2 - druga dana

1. Operacja zamkniecia
	EX => zamyka proces, kill program

2. Operacje logiczne
	MV nazwa_rejestru nazwa_rejestru/liczba => D1 <- D2
	EQ nazwa_rejestru nazwa_rejestru		=> true if d1 == d2, else false
	JP gdzie								=> jump D1
	JT gdzie								=> if true jump D1
	JF gdzie								=> if false jump D1

3. Operacje arytmetyczne 
	AD nazwa_rejestru liczba/nazwa_rejestru => D1 += D2
	ML nazwa_rejestru liczba/nazwa_rejestru => D1 *= D2
	SB nazwa_rejestru liczba/nazwa_rejestru => D1 -= D2
	DV nazwa_rejestru liczba/nazwa_rejestru => D1 /= D2
	IC nazwa_rejestru						=> D1++
	DC nazwa_rejestru						=> D1--

4. Operacje wykonywane na dysku
	OF nazwa_pliku							=> otwarcie pliku
	ZF nazwa_pliku							=> zamkniecie pliku
	CF nazwa_pliku							=> utworzenie pliku
	WF nazwa_pliku dane						=> zapis do pliku
	PF nazwa_pliku							=> Print File
	DF nazwa_pliku							=> Delete File
	RF nazwa_pliku_stara nazwa_pliku_nowa	=> Rename File
	AF nazwa_pliku dane						=> Add to File. Dopisuje do pliku
	LF										=> wy�wietla pliki na dysku
	PD										=> wy�wietla cala zawartosc dysku

5. Operacje wykonywane na komunikatach
	SC nazwa_procesu						=> Send communicate
	RC nazwa_procesu						=> Receive communicate
	

6. Operacje wykonywane na procesach
	CP nazwa_procesu						=> Create a new process
	DP nazwa_procesu						=> Delete a process
	AP										=> Print active process
	RP										=> print ready process

7. Operacje wykonywane na pamieci RAM
	SR										=> Show RAM
	SF										=> Show FIFO
*/