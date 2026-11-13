#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
#include <algorithm>

// ================== ENUMS ==================
enum class StatutReservation { EN_ATTENTE, CONFIRMEE, ANNULEE, EXPIREE };
enum class StatutPaiement { EN_ATTENTE, VALIDE, REMBOURSE };
enum class StatutVol { PROGRAMME, COMPLET, ANNULE };

// ================== CLASSES ==================
class Passager {
private:
    int id; std::string nom; std::string email; std::string piece;
public:
    Passager(int i, std::string n, std::string e, std::string p) : id(i), nom(n), email(e), piece(p) {}
    int getId() const { return id; }
    std::string getNom() const { return nom; }
    std::string getEmail() const { return email; }
    std::string getPiece() const { return piece; }
    void afficher() const { std::cout << "[" << id << "] " << nom << " | " << email << " | " << piece << std::endl; }
};

class Vol {
private:
    int id; std::string num; int placesTot; int placesDispo; double prix; StatutVol statut;
public:
    Vol(int i, std::string n, int p, double pr) : id(i), num(n), placesTot(p), placesDispo(p), prix(pr), statut(StatutVol::PROGRAMME) {}
    int getId() const { return id; }
    std::string getNum() const { return num; }
    int getPlacesDispo() const { return placesDispo; }
    double getPrix() const { return prix; }
    bool reserver() { if(placesDispo > 0) { placesDispo--; if(placesDispo==0) statut=StatutVol::COMPLET; return true; } return false; }
    void liberer() { placesDispo++; if(statut==StatutVol::COMPLET) statut=StatutVol::PROGRAMME; }
    void afficher() const { std::cout << "[" << id << "] Vol " << num << " | Prix: " << prix << " FCFA | Places: " << placesDispo << "/" << placesTot << std::endl; }
};

class Reservation {
private:
    int id; int idPassager; int idVol; double prix; StatutReservation statut;
public:
    Reservation(int i, int ip, int iv, double pr) : id(i), idPassager(ip), idVol(iv), prix(pr), statut(StatutReservation::EN_ATTENTE) {}
    
    // CORRECTION : Ajout des getters manquants
    int getId() const { return id; }
    int getIdVol() const { return idVol; }
    int getIdPassager() const { return idPassager; }
    StatutReservation getStatut() const { return statut; }
    
    void confirmer() { statut = StatutReservation::CONFIRMEE; }
    void annuler() { statut = StatutReservation::ANNULEE; }
    void afficher() const {
        std::string s = (statut==StatutReservation::EN_ATTENTE)?"EN_ATTENTE":(statut==StatutReservation::CONFIRMEE)?"CONFIRMEE":"ANNULEE";
        std::cout << "[" << id << "] Passager:" << idPassager << " Vol:" << idVol << " | " << prix << " FCFA | " << s << std::endl;
    }
};

class Paiement {
private:
    int id; int idReservation; double montant; StatutPaiement statut; std::string numBillet;
public:
    Paiement(int i, int ir, double m) : id(i), idReservation(ir), montant(m), statut(StatutPaiement::EN_ATTENTE) {}
    void valider() { statut = StatutPaiement::VALIDE; numBillet = "BIL" + std::to_string(id); }
    std::string getBillet() { return numBillet; }
    int getIdRes() const { return idReservation; }
};

// ================== GESTIONNAIRE ==================
class Agence {
private:
    std::vector<Passager> passagers;
    std::vector<Vol> vols;
    std::vector<Reservation> reservations;
    std::vector<Paiement> paiements;
    int cpt = 1;
public:
    Agence() { vols.emplace_back(cpt++, "AF501", 10, 250000); vols.emplace_back(cpt++, "ET902", 5, 180000); }

    void enregistrerPassager() {
        std::string n,e,p; std::cout << "Nom: "; std::cin >> n; std::cout << "Email: "; std::cin >> e; std::cout << "N° Piece: "; std::cin >> p;
        for(auto& ps : passagers) if(ps.getEmail()==e || ps.getPiece()==p) throw std::runtime_error("Passager deja existant");
        passagers.emplace_back(cpt++, n, e, p); std::cout << "Passager enregistre ID: " << cpt-1 << std::endl;
    }
    
    void ajouterVol() {
        std::string num; int places; double prix;
        std::cout << "Numero Vol: "; std::cin >> num; std::cout << "Nb Places: "; places = saisirInt(); std::cout << "Prix: "; prix = saisirDouble();
        vols.emplace_back(cpt++, num, places, prix); std::cout << "Vol ajoute ID: " << cpt-1 << std::endl;
    }

    void afficherVols() const { if(vols.empty()) std::cout << "Aucun vol\n"; for(auto& v : vols) v.afficher(); }

    void rechercherVol() {
        std::string mot; std::cout << "Numero a rechercher: "; std::cin >> mot;
        for(auto& v : vols) if(v.getNum().find(mot) != std::string::npos) v.afficher();
    }

    void effectuerReservation() {
        int idP, idV; std::cout << "ID Passager: "; idP = saisirInt(); std::cout << "ID Vol: "; idV = saisirInt();
        auto it = std::find_if(vols.begin(), vols.end(), [idV](Vol& v){ return v.getId()==idV; });
        if(it==vols.end()) throw std::runtime_error("Vol introuvable");
        if(!it->reserver()) throw std::runtime_error("Vol complet");
        reservations.emplace_back(cpt++, idP, idV, it->getPrix()); std::cout << "Reservation creee ID: " << cpt-1 << std::endl;
    }

    // CORRECTION ICI
    void enregistrerPaiement() {
        int idR; std::cout << "ID Reservation: "; idR = saisirInt();
        auto it = std::find_if(reservations.begin(), reservations.end(), [idR](Reservation& r){ return r.getId()==idR; });
        if(it==reservations.end()) throw std::runtime_error("Reservation introuvable");
        if(it->getStatut() == StatutReservation::CONFIRMEE) throw std::runtime_error("Reservation deja payee");
        
        auto itVol = std::find_if(vols.begin(), vols.end(), [idV=it->getIdVol()](Vol& v){ return v.getId()==idV; });
        if(itVol==vols.end()) throw std::runtime_error("Vol associe introuvable");
        
        paiements.emplace_back(cpt++, idR, itVol->getPrix()); 
        paiements.back().valider(); 
        it->confirmer();
        std::cout << "Paiement valide. Billet: " << paiements.back().getBillet() << std::endl;
    }

    // CORRECTION ICI
    void annulerReservation() {
        int idR; std::cout << "ID Reservation: "; idR = saisirInt();
        auto it = std::find_if(reservations.begin(), reservations.end(), [idR](Reservation& r){ return r.getId()==idR; });
        if(it==reservations.end()) throw std::runtime_error("Reservation introuvable");
        
        it->annuler();
        auto itV = std::find_if(vols.begin(), vols.end(), [idV=it->getIdVol()](Vol& v){ return v.getId()==idV; });
        if(itV!=vols.end()) itV->liberer();
        std::cout << "Reservation annulee et place liberee" << std::endl;
    }

    void consulterReservations() const { if(reservations.empty()) std::cout << "Aucune reservation\n"; for(auto& r : reservations) r.afficher(); }

    void infoReservation() {
        int idR; std::cout << "ID Reservation: "; idR = saisirInt();
        auto it = std::find_if(reservations.begin(), reservations.end(), [idR](Reservation& r){ return r.getId()==idR; });
        if(it==reservations.end()) throw std::runtime_error("Reservation introuvable");
        it->afficher();
    }

    static int saisirInt() {
        int val; while(!(std::cin >> val)) { std::cout << "Erreur saisie. Nombre: "; std::cin.clear(); std::cin.ignore(1000,'\n'); } return val;
    }
    static double saisirDouble() {
        double val; while(!(std::cin >> val)) { std::cout << "Erreur saisie. Nombre: "; std::cin.clear(); std::cin.ignore(1000,'\n'); } return val;
    }
};

// ================== MAIN ==================
int main() {
    Agence agence;
    int choix;

    do {
        std::cout << "\n========== MENU PRINCIPAL ==========" << std::endl;
        std::cout << "1. Enregistrer un passager" << std::endl;
        std::cout << "2. Ajouter un vol" << std::endl;
        std::cout << "3. Afficher les vols disponibles" << std::endl;
        std::cout << "4. Rechercher un vol" << std::endl;
        std::cout << "5. Effectuez une reservation" << std::endl;
        std::cout << "6. Enregistrer un payement" << std::endl;
        std::cout << "7. Annuler une reservation" << std::endl;
        std::cout << "8. Consultez les reservations" << std::endl;
        std::cout << "9. Afficher les informations d'une reservation" << std::endl;
        std::cout << "0. Quitter" << std::endl;
        std::cout << "Votre choix: ";
        
        choix = Agence::saisirInt();

        try {
            switch(choix) {
                case 1: agence.enregistrerPassager(); break;
                case 2: agence.ajouterVol(); break;
                case 3: agence.afficherVols(); break;
                case 4: agence.rechercherVol(); break;
                case 5: agence.effectuerReservation(); break;
                case 6: agence.enregistrerPaiement(); break;
                case 7: agence.annulerReservation(); break;
                case 8: agence.consulterReservations(); break;
                case 9: agence.infoReservation(); break;
                case 0: std::cout << "Fermeture..." << std::endl; break;
                default: std::cout << "Choix invalide" << std::endl;
            }
        } catch(const std::exception& e) {
            std::cerr << "ERREUR: " << e.what() << std::endl;
        }
    } while(choix != 0);

    return 0;
}
