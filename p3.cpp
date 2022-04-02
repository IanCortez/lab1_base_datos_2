// ---- TERMINADO ----
#include <iostream>
#include <string>
#include <vector>
#include <fstream>


struct Alumno {
	std::string Nombre;
	std::string Apellidos;
	std::string Carrera;
	float Mensualidad;
};


class VariableRecord {
private:
	std::string nombreArchivo;

public:
	VariableRecord() = default;
	VariableRecord(std::string nombreArchivo){
		this->nombreArchivo = nombreArchivo;
	};

	std::vector<Alumno> load(){
		std::ifstream inFile;
		inFile.open(this->nombreArchivo);

		if(!inFile.is_open()) throw "no se na podido abrir archivo";

		std::vector<Alumno> alumnos;

		std::string linea;
		std::getline(inFile, linea);	// Dummy getline to skip the field names
		while(std::getline(inFile, linea)){
			Alumno temp;
			int pos = 0;

			while(linea[pos] != '|'){
				temp.Nombre += linea[pos];
				++pos;
			}
			++pos;

			while(linea[pos] != '|'){
				temp.Apellidos += linea[pos];
				++pos;
			}
			++pos;

			while(linea[pos] != '|'){
				temp.Carrera += linea[pos];
				++pos;
			}
			++pos;

			std::string floatConv;
			for(int i=pos; i<linea.size(); ++i){
				floatConv += linea[i];
			}
			temp.Mensualidad = std::stof(floatConv);

			alumnos.push_back(temp);
		}
		inFile.close();

		return alumnos;
	}

	void add(Alumno record){
		std::ofstream outFile;
		outFile.open(this->nombreArchivo, std::ios::app);

		if(!outFile.is_open()) throw "no se na podido abrir archivo";

		outFile << record.Nombre << '|' << record.Apellidos << '|' << record.Carrera << '|' << record.Mensualidad;
		outFile << '\n';
		outFile << std::flush;

		outFile.close();
	}

	Alumno readRecord(int pos){
		std::ifstream inFile;
		inFile.open(this->nombreArchivo);

		if(!inFile.is_open()) throw "no se na podido abrir archivo";

		inFile.seekg(std::ios::beg);
		for(int i=0; i<pos; ++i){
			inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		std::string linea;
		std::getline(inFile, linea);

		Alumno temp;
		int index = 0;

		while(linea[index] != '|'){
			temp.Nombre += linea[index];
			++index;
		}
		++index;

		while(linea[index] != '|'){
			temp.Apellidos += linea[index];
			++index;
		}
		++index;

		while(linea[index] != '|'){
			temp.Carrera += linea[index];
			++index;
		}
		++index;

		std::string floatConv;
		for(int i=index; i<linea.size(); ++i){
			floatConv += linea[i];
		}
		temp.Mensualidad = std::stof(floatConv);

		inFile.close();

		return temp;
	}
};


Alumno crearAlumno(){
	Alumno temp;
	std::cin.ignore();

	std::cout << "Ingrese el nombre del alumno: ";
	std::getline(std::cin, temp.Nombre);

	std::cout << "Ingrese los apellidos del alumno: ";
	std::getline(std::cin, temp.Apellidos);

	std::cout << "Ingrese la carrera del alumno: ";
	std::getline(std::cin, temp.Carrera);

	std::cout << "Ingrese la mensualidad del alumno: ";
	std::cin >> temp.Mensualidad;

	return temp;
}


bool menu(VariableRecord registro){
	int opcionMenu;

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
			std::cout << alumno.Nombre << ' ' << alumno.Apellidos << ", "<< alumno.Carrera;
			std::cout << ", "<< alumno.Mensualidad << '\n';
		}
		alumnos.clear();

	} else if(opcionMenu == 2){
		Alumno nuevoRegistro = crearAlumno();
		registro.add(nuevoRegistro);
		std::cout << "\nAlumno agregado al registro\n";

	} else if(opcionMenu == 3){
		int alumnoPos;
		std::cout << "\n\nIngrese la posicion del alumno en el registro: ";
		std::cin >> alumnoPos;

		Alumno alumno = registro.readRecord(alumnoPos);

		std::cout << alumno.Nombre << ' ' << alumno.Apellidos << ", " << alumno.Carrera;
		std::cout << ", " << alumno.Mensualidad << '\n';

	} else {
		return false;
	}

	return true;
}


int main(){
	std::string filename = "datos3.txt";
	VariableRecord registro(filename);

	while(menu(registro)){
	}
	
	return 0;
}