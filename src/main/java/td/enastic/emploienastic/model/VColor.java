package td.enastic.emploienastic.model;

import android.graphics.Color;
import android.util.Log;

public class VColor {

    private int r;
    private int g;
    private int b;
    private int a;

    public VColor(int r, int g, int b, int a) {
        this.r = r;
        this.g = g;
        this.b = b;
        this.a = a;
    }

    public VColor(int r, int g, int b) {
        this.r = r;
        this.g = g;
        this.b = b;
    }

    public int red() {
        return r;
    }

    public void setRed(int r) {
        this.r = r;
    }

    public int green() {
        return g;
    }

    public void setGreen(int g) {
        this.g = g;
    }

    public int blue() {
        return b;
    }

    public void setBlue(int b) {
        this.b = b;
    }

    public int alpha() {
        return a;
    }

    public void setAlpha(int a) {
        this.a = a;
    }

    public String toCssRgbValue(){
        return "rgb("+r+","+g+","+b+")";
    }

    public static VColor fromString(String hexaneDecimalValue){
        int i = 1, r= 255, g = 255, b = 255;
        try {
            r = Integer.decode("0x"+hexaneDecimalValue.charAt(i++) + "" + hexaneDecimalValue.charAt(i++));
            g = Integer.decode("0x"+hexaneDecimalValue.charAt(i++) + "" + hexaneDecimalValue.charAt(i++));
            b = Integer.decode("0x"+hexaneDecimalValue.charAt(i++) + "" + hexaneDecimalValue.charAt(i++));
        }catch (IndexOutOfBoundsException | NumberFormatException e){
            Log.i("MYDEBUG", hexaneDecimalValue+" "+e.getMessage());
            e.printStackTrace();
        }
        return new VColor(r, g, b);
    }

    public String toSqliteFormat(){
        return r+":"+g+":"+b;
    }

    public static VColor fromSqliteValue(String str){
        int r = 255, g = 255, b = 255;
        try{
            String[] tab = str.split(":");
            r = Integer.parseInt(tab[0]);
            g = Integer.parseInt(tab[1]);
            b = Integer.parseInt(tab[2]);
        } catch (Exception e) {
            //e.printStackTrace();
        }
        return new VColor(r, g, b);
    }
}