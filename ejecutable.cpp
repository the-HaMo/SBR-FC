#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;

// ESTRUCTURA DE UN HECHO
struct Hecho {
    string id; 
    double fc;               
};

// ESTRUCTURA DE UNA BASE
struct Regla {
    string id; 
    string cond;     
    string accion;
    string op;        
    double fcAccion;       
    double fcRegla;             
};

vector<Regla> BC; // BASE DE CONOCIMIENTO
vector<Hecho> BH; // BASE DE HECHOS
ofstream salida; // fichero de salida

// funcion para leer la BC
void leerBC(const string &fileName) { 
        try {
        ifstream archivo(fileName);
         if (!archivo.is_open())
        {
           cerr << "No se pudo abrir el archivo BC";
           return;
        }

        string linea;
        getline(archivo, linea);
        int numeroReglas = stoi(linea);
        
        for (int i = 0; i < numeroReglas; i++)
        {
            getline(archivo, linea);
            int dospuntosPos = linea.find(":");
            int comaPos = linea.find(",");
            int igualPos = linea.find("=");
            int siPos = linea.find("Si");
            int entoncesPos = linea.find("Entonces");
            size_t charY = linea.find(" y ");
            size_t charO = linea.find(" o ");

            string id = linea.substr(0, dospuntosPos);
            double factorCerteza = stof(linea.substr(igualPos + 1));
            
            Regla regla;
            regla.id = id;
            regla.fcRegla = factorCerteza;
            regla.cond = linea.substr(siPos + 3, entoncesPos - siPos - 4);
            regla.accion = linea.substr(entoncesPos + 9, comaPos - (entoncesPos + 9));
            if (charY != string::npos){
                regla.op = "y";
            } 
            else if (charO != string::npos){
                regla.op = "o";
            }
            else {
                regla.op = ""; 
            }
            // cout << "Insertando... \n" << id << "\n" << regla.cond << "\n" << regla.op << "\n" << regla.accion << "\n" << factorCerteza << endl;
            BC.push_back(regla);
        }

        archivo.close();
        } catch (exception e){
            cerr << " Se ha producido un error";
            return;
        }
}

// funcion para leer la BH
void leerBH(const string &fileName, string &meta) 
{
        try {
        ifstream archivo(fileName);
         if (!archivo.is_open())
        {
           cerr << "No se pudo abrir el archivo BC";
           return;
        }

        string linea;
        getline(archivo, linea);
        int numHechos = stoi(linea);
        
        for (int i = 0; i < numHechos; i++)
        {
            getline(archivo, linea);
            string id = linea.substr(0, linea.find(","));
            string Stringfc = linea.substr(linea.find("=") + 1, linea.length());
            double fc = stod(Stringfc);
            Hecho hecho;
            hecho.id = id;
            hecho.fc = fc;

            bool Notexist = true;
                for (Hecho hecho : BH){
                    if (hecho.id == id){
                        hecho.fc = fc;
                        Notexist = false;
                        // cout << "Se modifico el factor de certeza de " << hecho.id << endl; 
                    }
                }
                if (Notexist){
                    BH.push_back(hecho);
                    // cout << "se inserto el hecho en la base " << hecho.id << endl;
                }
        }
        
        getline(archivo, linea);
        if (linea.find("Objetivo") == string::npos){
               cerr << "Error, no existe un objetivo." << endl;
               return;
        }
        getline(archivo, linea);
        meta = linea;
        // cout << "La meta es: " << meta;

        archivo.close();
        } catch (exception e){
            cerr << " Se ha producido un error";
        }
}

// función para equiparar el conjunto conflicto
vector<Regla> equiparar(string meta, vector<Regla> BC){ 
    vector<Regla> cjtoReglas;
    for (Regla regla : BC){
        if (regla.accion == meta){
            cjtoReglas.push_back(regla);
        }
    }
    return cjtoReglas;
}

// función auxiliar
void cojtoIniciales(const vector<string> &elementos)
{
        cout << " CC = {";
        salida << " CC = {";
        for (size_t i = 0; i < elementos.size(); i++)
        {
            cout << elementos[i];
            salida << elementos[i];
            if (i != elementos.size() - 1)
            {
                cout << ", ";
                salida << ", ";
            }
        }
        cout << "}" << endl;
        salida << "}" << endl;
}

// función auxiliar
void cjtoMetas(const vector<string> &metas)
{
    cout << "Nuevas metas = {";
    salida << "Nuevas metas = {";
    for (size_t i = 0; i < metas.size(); i++)
    {
        cout << metas[i];
        salida << metas[i];
        if (i != metas.size() - 1)
        {
            cout << ", ";
            salida << ", ";
        }
    }
    cout << "}" << endl;
    salida << "}" << endl;
}

// función auxiliar
vector<string> obtenerIds(const vector<Regla> &BC) 
{   
    vector<string> ids;
    for (Regla regla : BC)
    {
        ids.push_back(regla.id);
    }
    return ids;
}

// ver si la meta esta en la base de hechos
bool isBH(string meta, vector<Hecho> &BH) 
{
   for (Hecho hecho : BH){
            if (hecho.id == meta){
                return true;
            }
        }
    return false;
}

// dividir una regla
vector<string> dividir(const string &cadenaCondicion) {
    vector<string> division;
    size_t inicio = 0;
    size_t posDel;

    while (inicio < cadenaCondicion.size()) {
        size_t posDel1 = cadenaCondicion.find(" y ", inicio);
        size_t posDel2 = cadenaCondicion.find(" o ", inicio);

        if (posDel1 != string::npos && posDel2 != string::npos) {
            posDel = (posDel1 < posDel2) ? posDel1 : posDel2;
        } else if (posDel1 != string::npos) {
            posDel = posDel1;
        } else if (posDel2 != string::npos) {
            posDel = posDel2;
        } else {
            posDel = string::npos;
        }

        if (posDel == string::npos) {
            division.push_back(cadenaCondicion.substr(inicio));
            break;
        }

        division.push_back(cadenaCondicion.substr(inicio, posDel - inicio));
        if (posDel == posDel1) {
            inicio = posDel + 3; // " y " tiene 3 caracteres
        } else {
            inicio = posDel + 3; // " o " tiene 3 caracteres
        }
    }

    return division;
}

// para añadir la meta 
void anadirMeta(string meta, double fc)
{
    Hecho hecho;
    hecho.id = meta;
    hecho.fc = fc;
    if (!isBH(meta, BH))
    {
        BH.push_back(hecho);
    }
    else
    {
        for (Hecho hecho : BH){
            if (hecho.id == meta){
                hecho.fc = fc;
            }
        }
    }
}

// función que realiza el proceso
bool verificar(string meta)
{
       
    if (isBH(meta, BH)){
        for (Hecho hecho : BH){
            if (hecho.id == meta){
                cout << "Meta alcanzada: " << meta << ". con FC: " << hecho.fc << endl;
                salida << "Meta alcanzada: " << meta << ". con FC: " << hecho.fc << endl;   
                return true;
            }
        }
    }

    vector<Regla> cc = equiparar(meta, BC);
    if (cc.size()){
    cout << "Comprobando la meta del programa: " << meta << endl;
    salida << "Comprobando la meta del programa: " << meta << endl;
    cojtoIniciales(obtenerIds(cc));
    
    size_t i = 0;
    while (i < cc.size()){
        Regla &regla = cc[i]; 
        cout << "Regla seleccionada: " << regla.id << endl;
        salida << "Regla seleccionada: " << regla.id << endl;
        vector<string> nuevasMetas = dividir(regla.cond);
        cjtoMetas(nuevasMetas);

        bool verificado = true;
        size_t j = 0;
        while (j < nuevasMetas.size() && verificado){
            string nuevaMeta = nuevasMetas[j];
            cout << "Seleccionada la nueva meta: " << nuevaMeta << endl;
            salida << "Seleccionada la nueva meta: " << nuevaMeta << endl;
            verificado = verificar(nuevaMeta);
            j++;
        }

        if (verificado) {
        cout << regla.id << " (regla activada)" << endl;
        salida << regla.id << " (regla activada)" << endl;
        // CASO 1
        double factorCerteza = 0;
        double factorCertezaAnterior = 0;
        
        for (Hecho &hecho : BH){
            if (hecho.id == nuevasMetas[0]){
                factorCertezaAnterior = hecho.fc;
                break;
            }
        }

        string op = regla.op;

        for (size_t j = 1; j < nuevasMetas.size(); j++) {
            string antecedente = nuevasMetas[j];
            for (size_t k = 0; k < BH.size(); k++) {
                if (BH[k].id == antecedente) {
                    factorCerteza = BH[k].fc;
                    break;
                }
            }
            if (op == "y") {
                factorCertezaAnterior = min(factorCertezaAnterior, factorCerteza);
            }
            else if (op == "o") {
                factorCertezaAnterior = max(factorCertezaAnterior, factorCerteza);
            }
        }

        regla.fcAccion = factorCertezaAnterior;
        salida << "\tCaso 1: (" << regla.cond << "), FC: " << regla.fcAccion << endl;
        // CASO 3
        if (regla.fcAccion >= 0) {
            regla.fcAccion = regla.fcRegla * regla.fcAccion;
        }
        else {
            regla.fcAccion = regla.fcRegla * max(0.0, regla.fcAccion);
        }

        salida << "\tCaso 3: (" << regla.cond << "), FC: " << regla.fcAccion << endl;
    }
        i++;
    }
    if (cc.size() > 1)
    {
        double fcA = cc.front().fcAccion;
        cc.erase(cc.begin());

        while (!cc.empty())
        {
            double fcAct = cc.front().fcAccion;
            cc.erase(cc.begin());
            
            if (fcAct >= 0 && fcA >= 0){
                fcA = fcA + fcAct * (1 - fcA);
            } else if (fcA <= 0 && fcAct <= 0){
                fcA = fcA + fcAct * (1 + fcA);
            } else {
                fcA = (fcA + fcAct) / (1 - min(abs(fcA), abs(fcAct)));
            }
        }

        anadirMeta(meta, fcA);
        cout << "Caso 2: " << meta << ", FC: " << fcA << endl;
        cout << "Se ha alcanzado la meta " << meta << " con FC: " << fcA << endl;
        salida << "\tCaso 2: " << meta << ", FC: " << fcA << endl;
        salida << "Se ha alcanzado la meta " << meta << " con FC: " << fcA << endl;
        
        return true;

    } else {

        double fc = cc.front().fcAccion;
        anadirMeta(meta, fc);

        cout << "Se ha alcanzado la meta " << meta << "con FC: " << fc << endl;
        salida << "Se ha alcanzado la meta " << meta << "con FC: " << fc << endl;
        return true;
    }
    }

    return false;
}

// función principal
bool encadenamientoHaciaAtras(string &meta)
{       
     bool verificado = verificar(meta);
    if (verificado){

        cout << "--- !META ALCANZADA! ---" << endl;
        salida << "--- !META ALCANZADA! ---" <<endl;
        
        for (Hecho hecho : BH){
            if (hecho.id == meta){
                cout << "Resultado final para lograr la meta:  " <<  meta << " es FC= " << hecho.fc << endl;
                salida << "Se alcanzo la meta " <<  meta << " con un FC= " << hecho.fc << endl;
                return true;
            }
        }
        cout << "Error con la BH" << endl;
        return false;
    }
    else
    {
        cout << "--- META NO ALCANZADA ---" << endl;
        salida << "--- !META ALCANZADA! ---" <<endl;
        return false;
    }
}


int main(int argc, char *argv[]){

    if (argc != 4){
        cerr << "El Formato es el siguiente: ./ejecutable {Base de Conocimiento} {Base de Hechos} \"FicheroSalida.txt\"";
        return -1;
    }

    string BaseC = argv[1];
    string BaseH = argv[2];
    string ficheroName = argv[3];
    string meta;
    salida.open(ficheroName);

    if (!salida.is_open()){
        cerr << "Error al abrir el fichero de salida " << ficheroName << endl;
        return -1;
    }

    salida << "Base de Conocimiento utilizada: \"" << BaseC << "\" y Base de Hechos utilizada: \"" << BaseH << "\"" << endl;
    
    leerBC(BaseC);
    leerBH(BaseH, meta);
    
    salida << "El objetivo a encontrar es: " << meta << endl;
    salida << "\n--- COMIENZA EL ALGORITMO SBR-FC ---\n" << endl;
    cout << "--- COMIENZA EL ALGORITMO SBR-FC ---" << endl;
    
    bool resultado = encadenamientoHaciaAtras(meta);

    if (resultado){
        cout << "Se ha terminado el proceso con exito! "  << endl;
        salida << "Resultado: Proceso existoso" << endl;
    } else {
        cout << "No existe ninguna configuración posible. " << endl;
        salida << "Resultado: Proceso fallido. " << endl;
    }

    salida << "\n--- FIN DEL ALGORITMO SBR-FC ---" << endl;
    cout << "--- FIN DEL ALGORITMO SBR-FC ---" << endl;
    salida.close();
}
