// ---- TERMINADO ----
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

const int CODIGO_SIZE = 5;
const int NOMBRE_SIZE = 11;
const int APELLIDOS_SIZE = 20;
const int CARRERA_SIZE = 15;


struct Alumno {
	char codigo[CODIGO_SIZE];
	char nombre[NOMBRE_SIZE];
	char apellidos[APELLIDOS_SIZE];
	char carrera[CARRERA_SIZE];
};


class FixedRecord {
private:
	std::string nombreArchivo;

public:
	FixedRecord() = default;
	FixedRecord(std::string nombreArchivo){
		this->nombreArchivo = nombreArchivo;
	};

	std::vector<Alumno> load(){
		std::ifstream inFile;
		inFile.open(this->nombreArchivo);

		if(!inFile.is_open()) throw "no se na podido abrir archivo";

		std::vector<Alumno> alumnos;

		Alumno temp;
		while(inFile.read((char*) &temp, sizeof(Alumno))){
			alumnos.push_back(temp);
		}
		inFile.close();

		return alumnos;
	}

	void add(Alumno record){
		std::ofstream outFile;
		outFile.open(this->nombreArchivo, std::ios::app);

		if(!outFile.is_open()) throw "no se na podido abrir archivo";

		outFile.write((char*) &record, sizeof(Alumno));
		outFile << '\n';
		outFile << std::flush;

		outFile.close();
	}

	Alumno readRecord(int pos){
		std::ifstream inFile;
		inFile.open(this->nombreArchivo);

		if(!inFile.is_open()) throw "no se na podido abrir archivo";

		inFile.seekg(std::ios::beg);
		for(int i=0; i<pos-1; ++i){
			inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		Alumno temp;
		inFile.read((char*) &temp, sizeof(Alumno));

		inFile.close();

		return temp;
	}
};


Alumno crearAlumno(){
	Alumno temp;
	std::string input;

	std::cin.ignore();

	std::cout << "\nIngrese el codigo del alumno: ";
	std::getline(std::cin, input);
	for(int i=0; i<CODIGO_SIZE; ++i){
		if(i < input.size()) temp.codigo[i] = input[i];
		else temp.codigo[i] = ' ';
	}

	std::cout << "Ingrese el nombre del alumno: ";
	std::getline(std::cin, input);
	for(int i=0; i<NOMBRE_SIZE; ++i){
		if(i < input.size()) temp.nombre[i] = input[i];
		else temp.nombre[i] = ' ';
	}

	std::cout << "Ingrese los apellidos del alumno: ";
	std::getline(std::cin, input);
	for(int i=0; i<APELLIDOS_SIZE; ++i){
		if(i < input.size()) temp.apellidos[i] = input[i];
		else temp.apellidos[i] = ' ';
	}

	std::cout << "Ingrese la carrera del alumno: ";
	std::getline(std::cin, input);
	for(int i=0; i<CARRERA_SIZE; ++i){
		if(i < input.size()) temp.carrera[i] = input[i];
		else temp.carrera[i] = ' ';
	}

	return temp;
}


void showAlumno(Alumno alumno){
	// Mostrar el codigo del alumno
	for(int i=0; i<CODIGO_SIZE; ++i)
		std::cout << alumno.codigo[i];

	// Mostrar el nombre del alumno
	for(int i=0; i<NOMBRE_SIZE; ++i)
		std::cout << alumno.nombre[i];
	
	// Mostrar los apellidos del alumno
	for(int i=0; i<APELLIDOS_SIZE; ++i)
		std::cout << alumno.apellidos[i];

	// Mostrar la carrera del alumno
	for(int i=0; i<CARRERA_SIZE; ++i)
		std::cout << alumno.carrera[i];
	// std::cout <<  '\n';
}


bool menu(FixedRecord registro){
	int opcionMenu;

	// Menu
	std::cout << "\n\n\n\n------MENU------\n\n";
	std::cout << "Ingrese la opcion a realizar: ";
	std::cout << "\n1. Obtener todos los alumnos registrados\n";
	std::cout << "2. Registrar un nuevo alumno\n";
	std::cout << "3. Obtener el registro de un alumno\n";
	std::cout << "Cualquier otro valor para salir del menu\n";
	std::cout << "Opcion: ";
	std::cin >> opcionMenu;


	if(opcionMenu == 1){
		std::vector<Alumno> alumnos = registro.load();

		std::cout << '\n';
		for(Alumno alumno: alumnos){
			showAlumno(alumno);
		}
		alumnos.clear();

	} else if(opcionMenu == 2){
		Alumno nuevoRegistro = crearAlumno();
		registro.add(nuevoRegistro);
		std::cout << "\nAlumno agregado al registro\n";

	} else if(opcionMenu == 3){
		int alumnoPos;
		std::cout << "\nIngrese la posicion del alumno en el registro: ";
		std::cin >> alumnoPos;

		Alumno alumno = registro.readRecord(alumnoPos);
		showAlumno(alumno);

	} else {
		return false;
	}

	return true;
}


int main(){
	// Variables
	std::string filename = "datos1.txt";
	FixedRecord registro(filename);

	while(menu(registro)){
	}

	return 0;
}