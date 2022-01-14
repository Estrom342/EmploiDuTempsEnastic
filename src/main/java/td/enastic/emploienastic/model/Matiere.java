package td.enastic.emploienastic.model;

import android.graphics.Color;
import android.os.Build;
import android.util.Log;
import androidx.annotation.RequiresApi;

public class Matiere {

    private String m_name = "";
    private String m_comment="";
    private String m_couleur;
    private int m_id;
    private int m_idEmploi;
    private int m_numeroCase;
    private Teacher m_teacher= new Teacher();
    private String m_niveau;
    private String m_type;
    private String m_teacherName;
    private VColor m_color;

    public Matiere(String name){
        this.m_name = name;
        m_type="CM";
        m_teacherName="";
        m_color = new VColor(255, 255, 255);
        m_numeroCase = m_id = m_idEmploi = -1;
        m_comment = "";
    }

    public String niveau() {
        return m_niveau;
    }

    public void setNiveau(String m_niveau) {
        this.m_niveau = m_niveau;
    }

    public String couleur() {
        return m_couleur;
    }

    public void setCouleur(String m_couleur) {
        this.m_couleur = m_couleur;
    }

    public String teacherName() {
        return m_teacherName;
    }

    public void setTeacherName(String m_teacherName) {
        this.m_teacherName = m_teacherName;
    }

    public Teacher teacher() {
        return m_teacher;
    }

    public void setTeacher(Teacher teacher) {
        this.m_teacher = teacher;
        if(teacher!=null)
        m_teacherName = teacher.getGrade()+" "+teacher.getNom()+"<br/>"+
                teacher.getPrenom();
    }

    public String name() {
        return m_name;
    }
    public void setName(String m_name) {
        this.m_name = m_name;
    }

    public String comment() {
        return m_comment;
    }

    public void setComment(String m_comment) {
        this.m_comment = m_comment;
    }

    public String type() {
        return m_type;
    }

    public void setType(String m_type) {
        this.m_type = m_type;
    }

    public int id() {
        return m_id;
    }

    public void setId(int m_id) {
        this.m_id = m_id;
    }

    public int idemploi() {
        return m_idEmploi;
    }

    public void setIdEmploi(int m_idEmploi) {
        this.m_idEmploi = m_idEmploi;
    }

    public int numeroCase() {
        return m_numeroCase;
    }

    public void setNumeroCase(int m_numeroCase) {
        this.m_numeroCase = m_numeroCase;
    }

    public VColor color() {
        return m_color;
    }

    public void setColor(VColor m_color) {
        this.m_color = m_color;
    }

    public String toString(){
        return m_type+"\n"+
                m_name+ ":"+numeroCase()+"\n"+
                m_teacher.toString()+
                m_color.toString()+"\n"+
                (m_comment.isEmpty() ? Constant.DEFAULT_COMMENT : m_comment)+"\n";
    }

    public static Matiere fromString(String string){
        Matiere m = new Matiere("");
        try {
            String[] tab = string.split("\n");
            int i = 0;
            m.setType(tab[i++]);
            String name = tab[i++];
            m.setTeacher(Teacher.fromString(tab[i++] + "\n" + tab[i++]+ "\n" + tab[i++]));
            try {
                m.setColor(VColor.fromString(tab[i++]));
            } catch (NumberFormatException | IndexOutOfBoundsException e) {
                System.out.println("Matiere.fromString: " + e.getMessage());
            }
            m.setComment((tab[i++].equals(Constant.DEFAULT_COMMENT) ? "" : tab[--i]));
            m.setName(name.split(":")[0]);
            m.setNumeroCase(Integer.parseInt(name.split(":")[1]));
        }catch (IndexOutOfBoundsException e){
            System.out.println("Matiere.fromString: "+e.getMessage());
        }
        return m;
    }

    public String toStyleSheese() {
        return "background-color:" + m_color.toString() + " border: 1px solid black; " +
                "text-align: center; vertical-align: middle;";
    }

    public String toHtml()
    {
        String txt =(m_type.equals("CM") ?"" : m_type) + "<br/>" +
                m_name + "<br/>" +
                m_teacherName+ "<br/>";
        if(m_comment.length()>0)
            txt+= "("+m_comment+")<br/>";
        return "<td style=\" border: 2px solid black; text-align: center; background: "+ m_color.toCssRgbValue()+"\"><div>" + txt + "</div></td>";
    }

    public int getNumeroCase() {
        return m_numeroCase;
    }
};
