package td.enastic.emploienastic.model;

import android.util.Log;

import java.util.Calendar;

public class VDate {

    private int m_day;
    private int m_mouth;
    private int m_year;
    public static String[] MOUTHS = {"Janvier", "FEVRIERS", "MARS", "AVRIL", "Main", "Juin",
        "Juillet", "Aout", "Septembre", "Cotobre", "Novenbre", "Decembre"};
    public static String[] DayOfWeek = {"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche"};

    public VDate(){
        Calendar c = Calendar.getInstance();
        m_day = c.get(Calendar.DATE);
        m_mouth = c.get(Calendar.MONTH);
        m_year = c.get(Calendar.YEAR);
    }

    public VDate(int y, int m, int d){
        m_year = y;
        m_mouth = m;
        m_day = d;
    }

    public boolean isValid(){
        return m_year>=0 && 0<= m_mouth && m_mouth<= 12 && 0<= m_day && m_day<=31;
    }

    @Override
    public String toString() {
        if(!isValid()) return "0000:00:00";
        return  m_day + ":" + m_mouth+ ":"+ m_year;
    }
    public static VDate fromString(String string){
        String[] tab = string.split("\n")[0].split(":");
        VDate d = new VDate();
        try{
            int y = Integer.parseInt(tab[0]);
            int m = Integer.parseInt(tab[1]);
            int day = Integer.parseInt(tab[2]);
            d = new VDate(y, m, day);
        }catch (NumberFormatException | IndexOutOfBoundsException e){
            System.out.println("Error at VDate.fromString: "+string+" "+string.length()+" ;"+e.getMessage());
        }
        return d;
    }

    public String toSqlFormat(){
        return  m_year + ":" + m_mouth+ ":"+ m_day+"\n";
    }

    public int day() {
        return m_day;
    }
    public int mouth() {
        return m_mouth;
    }
    public int year() {
        return m_year;
    }
}
