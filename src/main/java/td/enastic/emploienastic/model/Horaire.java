package td.enastic.emploienastic.model;

public class Horaire {

    private VTime m_heureDebut; private VTime m_heureFin;
    int m_idEmploi, m_numeroCase;

    public Horaire(){
        m_heureDebut = new VTime();
        m_heureFin = new VTime();
        m_idEmploi = -1;
        m_numeroCase = -1;
    }

    public Horaire(int numCase){
        m_heureDebut = new VTime();
        m_heureFin = new VTime();
        m_idEmploi = -1;
        m_numeroCase = numCase;
    }

    String durationTOString(){
        int duration = (m_heureFin.hour() - m_heureDebut.hour())*60+
                (m_heureFin.minute() - m_heureDebut.minute());
        return (duration >= 60 ? duration/60 + "h" : "")+
                duration%60+"m";
    }
    @Override
    public String toString(){
        m_heureDebut = new VTime(m_heureDebut.hour(), m_heureDebut.minute(), m_heureDebut.second(), m_numeroCase);
        return m_heureDebut.toString()+m_heureFin.toString();
    }

    public static Horaire fromString(String string){
        String[] tab = string.split("\n");
        Horaire h = new Horaire();
        try{
            h.setHeureDebut(VTime.fromString(tab[0].toString()));
            h.setHeureFin(VTime.fromString(tab[1].toString()));
            h.setNumeroCase(h.heureDebut().getMs());
        }catch (IndexOutOfBoundsException e){
            System.out.println("Horaire.fromString: "+e.getMessage());
        }
        return h;
    }

    public String toHtml(){
        return m_heureDebut.hour()+"h"+m_heureDebut.minute()+"m<br/>à</br>"+
                m_heureFin.hour()+"h"+m_heureFin.minute()+"m";
    }

    public VTime heureDebut(){
        return m_heureDebut;
    }

    void setHeureDebut(VTime newHeureDebut){
        m_heureDebut = newHeureDebut;
    }

    public VTime heureFin(){
        return m_heureFin;
    }

    void setHeureFin(VTime newHeureFin){
        m_heureFin = newHeureFin;
    }

    int idEmploi(){
        return m_idEmploi;
    }

    void setIdEmploi(int newIdEmploi){
        m_idEmploi = newIdEmploi;
    }

    public int numeroCase(){
        return m_numeroCase;
    }

    public void setNumeroCase(int i) {
        m_numeroCase = i;
    }
    /*


void Horaire::setNumeroCase(int newNumeroCase)
{
    m_numeroCase = newNumeroCase;
    switch (newNumeroCase) {
    case 1:
        m_heureDebut = QTime(7, 30);
        m_heureFin = QTime(9, 30);
        break;
    case 2:
        m_heureDebut = QTime(9, 30);
        m_heureFin = QTime(9, 40);
        break;
    case 3:
        m_heureDebut = QTime(9, 40);
        m_heureFin = QTime(12, 40);
        break;
    case 4:
        m_heureDebut = QTime(12, 40);
        m_heureFin = QTime(13, 40);
        break;
    case 5:
        m_heureDebut = QTime(13, 40);
        m_heureFin = QTime(15, 40);
        break;
    default:
        m_heureDebut = QTime::currentTime();
        m_heureFin = QTime::currentTime().addMSecs(360 * 2);
    }
}
*/
}
