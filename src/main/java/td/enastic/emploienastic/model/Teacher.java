package td.enastic.emploienastic.model;

public class Teacher {

    private String nom="";
    private String prenom="";
    private String grade="Mr";
    private String niveau="";
    private String matiere="";
    private int id;

    Teacher(){}

    public Teacher(String nom, String prenom, String grade, String niveau){
        this.nom = nom; this.prenom = prenom; this.grade = grade;
        this.niveau = niveau; this.id = -1;
    }

    String getNom(){
        return nom;
    }

    void setNom(String newNom){
        nom = newNom;
    }

    String getPrenom(){
        return prenom;
    }

    void setPrenom(String newPrenom){
        prenom = newPrenom;
    }

    String getGrade() {
        return grade;
    }

    void setGrade(String newGrade){
        grade = newGrade;
    }

    String getNiveau(){
        return niveau;
    }

    void setNiveau(String newNiveau){
        niveau = newNiveau;
    }

    String getNomMatiere(){
        return matiere;
    }

    void setNomMatiere(String newMatiere){
        matiere = newMatiere;
    }

    @Override
    public String toString() {
        return grade+"\n"+
                nom+"\n"+
                prenom+"\n";
    }

    public static Teacher fromString(String str){
        String[] tab = str.split("\n");
        Teacher t = new Teacher();
        try {
            String grade = tab[0]; t.setGrade(grade);
            String nom = tab[1]; t.setNom(nom);
            String prenom = tab[2]; t.setPrenom(prenom);
        }catch (Exception e){
            System.out.println("Teacher.fromString(String str): "+e.getMessage());
        }
        return t;
    }
    int getId(){
        return id;
    }

    void setId(int newId){
        id = newId;
    }

/*
QDataStream operator<<(QDataStream stream, Teacher  t){
    stream<<t.getGrade()<<t.getNom()<<t.getPrenom();
    return stream;
}
QDataStream operator>>(QDataStream stream, Teacher t){
   String grade, nom, prenom;
    stream>>grade>>nom>>prenom;
    t.setGrade(grade); t.setNom(nom); t.setPrenom(prenom);
    return stream;
}

*/
}
