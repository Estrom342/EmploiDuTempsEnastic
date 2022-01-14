package td.enastic.emploienastic.controler;

import td.enastic.emploienastic.model.EmploiDuTemp;
import td.enastic.emploienastic.model.VDate;

public interface SocketListener{
    void timeTableAvailable(EmploiDuTemp em);
    void status(String state); //Signal pour afficher le status a la barre de status
    void signatureAvailable(String level, String nomSalle, VDate dateDebut, VDate dateFin, byte[] signature);

}
