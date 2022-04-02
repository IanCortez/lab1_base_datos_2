#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <utility>


struct Matricula {
	// Peso total de cada registro (contando separaciones)
	std::string codigo;
	int ciclo;
	float mensualidad;
	std::string observaciones;

	
	int sizeMatricula(){
		int size = 0;
		size += codigo.length() * sizeof(char);
		size += sizeof(ciclo);
		size += sizeof(mensualidad);
		size += observaciones.length() * sizeof(char);
		return size;
	}


	char* empaquetar(){
		int size = sizeMatricula();
		char* buffer = new char[size];

		/*
		concatenar(buffer, codigo.c_str());
		concatenar(buffer, (char*) &ciclo);
		concatenar(buffer, (char*) &mensualidad);
		concatenar(buffer, observaciones.c_str());
		*/
		return buffer;
	}


	void desempaquetar(char* buffer){
		
	}
};


class VariableRecord {
private:
	std::string nombreArchivo;

public:
	VariableRecord() = default;
	VariableRecord(std::string nombreArchivo){
		this->nombreArchivo = nombreArchivo;
	};

	std::vector<Matricula> load(){
		// Leer del archivo de metadata
		std::ifstream inFile;
		inFile.open(this->nombreArchivo, std::ios::binary);
		
		if(!inFile.is_open()) throw "no se na podido abrir archivo";

		Matricula matricula;
		char *buffer = new char[matricula.sizeMatricula()];
		int numberBuffer;
		std::string holder;

		inFile.seekg(0, std::ios::end);
		int fileSize = inFile.tellg();
		inFile.seekg(0);

		// Cargar datos del binario
		std::vector<Matricula> matriculas;

		for(int i=0; i<fileSize; ++i){
			// Leer el separador del campo que indica el tamaño del campo en bytes
			inFile.read(reinterpret_cast<char*>(&numberBuffer), sizeof(int));
			
			// Leer el campo correspondiente
			inFile.read(buffer, numberBuffer);
			holder = "";
			for(int i=0; i<numberBuffer; ++i){
				holder += buffer[i];
			}
			matricula.codigo = holder;
			
			// Leer el separador del campo que indica el tamaño del campo en bytes
			inFile.read(reinterpret_cast<char*>(&numberBuffer), sizeof(int));

			// Leer el campo de ciclo
			inFile.read(reinterpret_cast<char*>(&matricula.ciclo), sizeof(int));

			// Leer el separador del campo que indica el tamaño del campo en bytes
			inFile.read(reinterpret_cast<char*>(&numberBuffer), sizeof(int));

			// Leer el campo de mensualidad 
			inFile.read(reinterpret_cast<char*>(&matricula.mensualidad), sizeof(float));


			// Leer el separador del campo que indica el tamaño del campo en bytes
			inFile.read(reinterpret_cast<char*>(&numberBuffer), sizeof(int));

			// Leer el campo de observaciones
			inFile.read(buffer, numberBuffer);
			holder = "";
			for(int i=0; i<numberBuffer; ++i){
				holder += buffer[i];
			}
			matricula.observaciones = holder;

			// agregar matricula al vector
			matriculas.push_back(matricula);
		}

		inFile.close();

		delete[] buffer;

		return matriculas;
	}


	void add(Matricula record){
		// Abrir y guardar un registro en el binary
		std::ofstream outFile;
		outFile.open(this->nombreArchivo, std::ios::binary | std::ios::app);

		if(!outFile.is_open()) throw "no se na podido abrir archivo";

		// Escribir el tamanio del codigo en el archivo
		int sz = record.codigo.length();
		outFile.write(reinterpret_cast<const char*>(&sz), sizeof(int));
		// Escribir el codigo en el archivo
		outFile.write(record.codigo.c_str(), record.codigo.length() * sizeof(char));

		// Escribir el tamanio del ciclo en el archivo
		sz = sizeof(int);
		outFile.write(reinterpret_cast<const char*>(&sz), sizeof(int));
		std::cout << 'a' << std::endl;
		// Escribir el ciclo en el archivo
		outFile.write(reinterpret_cast<const char*>(&record.codigo), sizeof(int));

		// Escribir el tamanio de la mensualidad en el archivo
		sz = sizeof(float);
		outFile.write(reinterpret_cast<const char*>(&sz), sizeof(float));
		// Escribir la mensualidad en el archivo
		outFile.write(reinterpret_cast<const char*>(&record.mensualidad), sizeof(float));

		// Escribir el tamanio de las observaciones en el archivo
		sz = record.observaciones.length();
		outFile.write(reinterpret_cast<const char*>(&sz), sizeof(record.observaciones.length()) * sizeof(char));
		// Escribir las observacines en el archivo
		outFile.write(record.observaciones.c_str(), record.observaciones.length() * sizeof(char));
		
		// Guardar el tamanio del archivo para indicar donde se encuentra el nuevo registro
		int dataPos = outFile.tellp();
		outFile.close();


		// Abrir y guardar un registro en el metadata
		outFile.open("metadata.txt", std::ios::app);

		if(!outFile.is_open()) throw "no se na podido abrir archivo";

		// Guardar el size del registro que es el size de los atributos mas 4 veces el size de un int para los separadores
		int dataSize = record.sizeMatricula() + sizeof(int) * 4; // arreglar desde aca
		
		outFile << dataPos << ' ' << dataSize;
		outFile << '\n';
		outFile << std::flush;

		outFile.close();
	}


	Matricula readRecord(int pos){
		// Abrir y cargar datos del metadata ile
		std::ifstream inFile;
		inFile.open("metadata.txt");

		if(!inFile.is_open()) throw "no se na podido abrir archivo";

		inFile.seekg(std::ios::beg);
		for(int i=0; i<pos-1; ++i){
			inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		int tamanio;
		int posicion;
		inFile.read((char*) &tamanio, sizeof(int));
		inFile.read((char*) &posicion, sizeof(int));

		inFile.close();

		
		// Abrir y cargar datos del file
		inFile.open(this->nombreArchivo, std::ios::binary);

		if(!inFile.is_open()) throw "no se na podido abrir archivo";

		Matricula matricula;
		char *buffer = new char[matricula.sizeMatricula()];
		char *numberBuffer = new char[sizeof(int)];
		int sizeField;
		std::string holder;

		// Cargar datos del binario
		std::vector<Matricula> matriculas;

		// Buscar el elemento guardado en la posicion del archivo correspondiente
		inFile.seekg(posicion * tamanio);

		// Leer el separador del campo que indica el tamaño del campo en bytes
		inFile.read(numberBuffer, sizeof(int));
		sizeField = std::atoi(numberBuffer);
		
		// Leer el campo correspondiente
		inFile.read(buffer, sizeField);
		for(int i=0; i<sizeField; ++i){
			holder += buffer[i];
		}
		matricula.codigo = holder;
		
		// Leer el separador del campo que indica el tamaño del campo en bytes
		inFile.read(numberBuffer, sizeof(int));
		sizeField = std::atoi(numberBuffer);

		// Leer el campo de ciclo
		inFile.read(buffer, sizeof(int));
		holder = "";
		for(int i=0; i<sizeField; ++i){
			holder += buffer[i];
		}
		matricula.ciclo = std::stoi(holder);

		// Leer el separador del campo que indica el tamaño del campo en bytes
		inFile.read(numberBuffer, sizeof(int));
		sizeField = std::atoi(numberBuffer);

		// Leer el campo de mensualidad 
		inFile.read(buffer, sizeof(float));
		holder = "";
		for(int i=0; i<sizeField; ++i){
			holder += buffer[i];
		}
		matricula.ciclo = std::stoi(holder);


		// Leer el separador del campo que indica el tamaño del campo en bytes
		inFile.read(numberBuffer, sizeof(int));
		sizeField = std::atoi(numberBuffer);

		// Leer el campo de observaciones
		inFile.read(buffer, sizeField);
		holder = "";
		for(int i=0; i<sizeField; ++i){
			holder += buffer[i];
		}
		matricula.observaciones = holder;

		return matricula;
	}
};


Matricula crearAlumno(){
	Matricula temp;

	std::cin.ignore();
	std::cout << "\nIngrese el codigo de alumno matricula: ";
	std::getline(std::cin, temp.codigo);

	std::cout << "Ingrese el ciclo de matricula: ";
	std::cin >> temp.ciclo;

	std::cout << "Ingrese la mensualidad para la matricula: ";
	std::cin >> temp.mensualidad;

	std::cin.ignore();
	std::cout << "Ingrese las observaciones para la matricula: ";
	std::getline(std::cin, temp.observaciones);

	return temp;
}


bool menu(VariableRecord registro){
	int opcionMenu;
	std::cout << "\n\n------MENU------\n\n";
	std::cout << "Ingrese la opcion a realizar: ";
	std::cout << "\n1. Obtener todos los registros de matricula\n";
	std::cout << "2. Registrar un nueva matricula\n";
	std::cout << "3. Obtener el registro de una matricula\n";
	std::cout << "Opcion: ";
	std::cin >> opcionMenu;


	// Opciones
	if(opcionMenu == 1){
		std::vector<Matricula> matriculas = registro.load();	// Obtener alumnos registrados

		std::cout << '\n';
		for(Matricula matricula: matriculas){
			std::cout << "Codigo: " << matricula.codigo << "\nCiclo: " << matricula.ciclo;
			std::cout << "\nMensualidad: " << matricula.mensualidad;
			std::cout << "\nObservaciones: " << matricula.observaciones << '\n';
		}
		matriculas.clear();

	} else if(opcionMenu == 2){
		Matricula nuevoRegistro = crearAlumno();
		registro.add(nuevoRegistro);
		std::cout << "\nMatricula agregado al registro\n";

	} else if(opcionMenu == 3){
		int matriculaPos;
		std::cout << "\nIngrese la posicion del alumno en el registro: ";
		std::cin >> matriculaPos;

		Matricula matricula = registro.readRecord(matriculaPos);
		std::cout << "Codigo: " << matricula.codigo << "\nCiclo: " << matricula.ciclo;
		std::cout << "\nMensualidad: " << matricula.mensualidad;
		std::cout << "\nObservaciones: " << matricula.observaciones << '\n';

	} else {
		return false;
	}
	
	return true;
}


int main(){
	std::string filename = "datos4.bin";
	VariableRecord registro(filename);

	while(menu(registro)){
	}

	return 0;
}