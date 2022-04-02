/*
	Errores:
	- Con readRecord los valores de ciclo y mensualidad no salen como se espera
	- Con delete, no funciona apropiadamente
*/


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

	int ciclo;
	float mensualidad;

	// Elementos de freelist
	int nextDel = 0;
};


class FixedRecord {
private:
	std::string nombreArchivo;
	Alumno header;


public:
	FixedRecord() = default;
	FixedRecord(std::string nombreArchivo){
		this->nombreArchivo = nombreArchivo;
		header.nextDel = -1;
	};


	std::vector<Alumno> load(){
		std::ifstream inFile;
		inFile.open(this->nombreArchivo, std::ios::binary);

		if(!inFile.is_open()) throw "no se na podido abrir archivo";

		std::vector<Alumno> alumnos;

		inFile.seekg(0, std::ios::end);
		int fileSize = inFile.tellg();
		inFile.seekg(0);

		Alumno temp;
		for(int i=0; i< fileSize/sizeof(Alumno)+1; ++i){
			inFile.read(reinterpret_cast<char*>(&temp.codigo), CODIGO_SIZE);
			inFile.read(reinterpret_cast<char*>(&temp.nombre), NOMBRE_SIZE);
			inFile.read(reinterpret_cast<char*>(&temp.apellidos), APELLIDOS_SIZE);
			inFile.read(reinterpret_cast<char*>(&temp.carrera), CARRERA_SIZE);
			inFile.read(reinterpret_cast<char*>(&temp.ciclo), sizeof(int));
			inFile.read(reinterpret_cast<char*>(&temp.mensualidad), sizeof(float));
			inFile.read(reinterpret_cast<char*>(&temp.nextDel), sizeof(int));
			if(temp.nextDel == 0) alumnos.push_back(temp);
		}
		inFile.close();

		return alumnos;
	}


	void add(Alumno record){
		std::ofstream outFile;
		outFile.open(this->nombreArchivo, std::ios::app | std::ios::binary);

		if(!outFile.is_open()) throw "no se na podido abrir archivo";

		outFile.write(reinterpret_cast<char*>(&record.codigo), CODIGO_SIZE);
		outFile.write(reinterpret_cast<char*>(&record.nombre), NOMBRE_SIZE);
		outFile.write(reinterpret_cast<char*>(&record.apellidos), APELLIDOS_SIZE);
		outFile.write(reinterpret_cast<char*>(&record.carrera), CARRERA_SIZE);
		outFile.write(reinterpret_cast<char*>(&record.ciclo), sizeof(int));
		outFile.write(reinterpret_cast<char*>(&record.mensualidad), sizeof(float));
		outFile.write(reinterpret_cast<char*>(&record.nextDel), sizeof(int));
		//outFile << '\n';
		outFile << std::flush;
		outFile.close();
	}


	Alumno readRecord(int pos){
		std::ifstream inFile;
		inFile.open(this->nombreArchivo, std::ios::binary);

		if(!inFile.is_open()) throw "no se na podido abrir archivo";

		Alumno temp;
		inFile.seekg(pos * sizeof(Alumno));
		inFile.read(reinterpret_cast<char*>(&temp.codigo), CODIGO_SIZE);
		inFile.read(reinterpret_cast<char*>(&temp.nombre), NOMBRE_SIZE);
		inFile.read(reinterpret_cast<char*>(&temp.apellidos), APELLIDOS_SIZE);
		inFile.read(reinterpret_cast<char*>(&temp.carrera), CARRERA_SIZE);
		inFile.read(reinterpret_cast<char*>(&temp.ciclo), sizeof(int));
		inFile.read(reinterpret_cast<char*>(&temp.mensualidad), sizeof(float));
		inFile.read(reinterpret_cast<char*>(&temp.nextDel), sizeof(int));
		inFile.close();

		return temp;
	}


	bool remove(int pos){
		std::fstream file;
		file.open(this->nombreArchivo, std::ios::in | std::ios::out | std::ios::binary);

		if(!file.is_open()) return false;

		// Crear una variable alumno
		Alumno alumno;

		// Buscar el alumno en la posicion relativa asignada
		file.seekg(pos * sizeof(Alumno));

		// Leer y guardar la info del alumno en la variable creada
		file.read((char*) &alumno, sizeof(Alumno));

		// Cambiarlo como final de la Free List
		alumno.nextDel = -1;

		// Guardar la data en el archivo
		file.write((char*) &alumno, sizeof(Alumno));

		// Crear una variable temporal para modificar la Free List
		Alumno temp = header;
		
		// Crear una variable temporarl para guardar el final antiguo de la free list
		Alumno oldEnd;

		// Recorrer toda la Free List hasta encontrar el final antiguo
		while(temp.nextDel != -1){
			oldEnd = temp;
			file.seekg(temp.nextDel * sizeof(Alumno));
			file.read((char*) &temp, sizeof(Alumno));
		}

		// Actualizar el final antiguo con el nuevo final al remover el valor de la lista
		temp.nextDel = pos;

		// Ir a la posicion con el final anterior de la free List
		file.seekg(oldEnd.nextDel * sizeof(Alumno));

		// Cambiar el final a la posicion del nuevo valor que se ha removido
		oldEnd.nextDel = pos;

		// Sobre escribir los datos
		file.write((char*) &temp, sizeof(Alumno));

		file.close();
		
		return true;
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

	std::cout << "Ingrese el ciclo del alumno: ";
	std::cin >> temp.ciclo;

	std::cout << "Ingrese la mensualidad del alumno: ";
	std::cin >> temp.mensualidad;

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

	// Mostrar el ciclo del alumno
	if(alumno.ciclo == 10) std::cout << alumno.ciclo << " ";
	else std::cout << alumno.ciclo << "  ";
	
	// Mostrar la mensualidad del alumno
	std::cout << alumno.mensualidad << '\n';
}



bool menu(FixedRecord registro){
	int opcionMenu;

	// Menu
	std::cout << "\n\n------MENU------\n\n";
	std::cout << "Ingrese la opcion a realizar: ";
	std::cout << "\n1. Obtener todos los alumnos registrados\n";
	std::cout << "2. Registrar un nuevo alumno\n";
	std::cout << "3. Obtener el registro de un alumno\n";
	std::cout << "4. Eliminar un registro de alumno\n";
	std::cout << "Cualquier otro valor para salir del menu\n";
	std::cout << "Opcion: ";
	std::cin >> opcionMenu;


	// Opciones
	if(opcionMenu == 1){
		std::vector<Alumno> alumnos = registro.load();	// Obtener alumnos registrados
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

	} else if(opcionMenu == 4) {
		int alumnoPos;
		std::cout << "\nIngrese la posicion del alumno a remover en el registro: ";
		std::cin >> alumnoPos;

		bool flagExito = registro.remove(alumnoPos);

		if(flagExito) std::cout << "\nSe ha removido el estudiante con exito\n";
		else std::cout << "\nNo se ha podido remover el indice\n";

	} else {
		return false;
	}

	return true;
}


int main(){
	// Variables
	std::string filename = "datos2.bin";
	FixedRecord registro(filename);

	while(menu(registro)){
	}

	
	return 0;
}