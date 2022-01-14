package td.enastic.emploienastic;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.text.Html;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentTransaction;
import td.enastic.emploienastic.controler.SocketListener;
import td.enastic.emploienastic.controler.StudentSocket;
import td.enastic.emploienastic.databinding.ActivityMainBinding;
import td.enastic.emploienastic.model.Constant;
import td.enastic.emploienastic.model.EmploiDuTemp;
import td.enastic.emploienastic.model.LocalDatabase;
import td.enastic.emploienastic.model.VDate;

import java.util.ArrayList;
import java.util.Stack;

public class MainActivity extends AppCompatActivity implements SocketListener {

    public int idCurrentFragment = R.layout.fragment_home;
    public final String CURRENT_ID_FRAGMNET_TEXT = "CURRENT_ID_TEXT";
    public final String LAST_INDEX_TIMETABLE = "LAST_INDEX_TIMETABLE";
    public static int RESULT_CODE_SETTING = R.layout.activity_setting;

    private ActivityMainBinding ui = null;
    private HomeFragment homeWidget = null;
    private HelpFragment helpWidget = null;
    private ListLevelFragment listLevelFragment = null;
    private TimeTableFragment timeTableFragment = null;
    private Stack<Integer> lsUi = new Stack<>();
    private ListLevelFragment.OnShowTimeTableListener chooserLevelListener;
    LocalDatabase database;
    StudentSocket studentSocket;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        contruct();
        if (savedInstanceState!=null){
            int lastIndex = savedInstanceState.getInt(LAST_INDEX_TIMETABLE, -100);
            timeTableFragment.setCurrentIndex(lastIndex != -100 ? lastIndex : timeTableFragment.getCurrentIndex());
            int id = savedInstanceState.getInt(CURRENT_ID_FRAGMNET_TEXT, R.layout.fragment_home);
            showFragment(id);
        }
    }

    void contruct(){
        database = new LocalDatabase(this, Constant.DATABASE_NAME, null, 1);
        ui = ActivityMainBinding.inflate(getLayoutInflater());
        homeWidget = new HomeFragment(this);
        timeTableFragment = new TimeTableFragment();
        helpWidget = new HelpFragment();

        ListLevelFragment.OnShowTimeTableListener chooserLevelListener = new ListLevelFragment.OnShowTimeTableListener() {
            @Override
            public void setCurrentIndex(int index) {
                showFragment(R.layout.fragment_timetable);
                timeTableFragment.loadTimeTable(index);
            }
        };
        listLevelFragment = new ListLevelFragment(chooserLevelListener);

        FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
        fragmentTransaction.replace(ui.centralWidget.getId(), homeWidget);
        fragmentTransaction.commit();
        setContentView(ui.getRoot());
        lsUi = new Stack<>();

        ArrayList<String> lsLevel = database.getLsLevel();
        ArrayList<EmploiDuTemp> lsTimeTable = new ArrayList<>();
        ArrayList<ListLevelFragment.Level> lSLevel = new ArrayList<>();
        Log.i("LEVEL", "Ls levele size = " + lsLevel.size());
        int indexLevel = 0;
        for (String level : lsLevel) {
            Log.i("LEVEL", level);
            EmploiDuTemp em = database.getLastEmploi(level);
            lsTimeTable.add(em);
            ListLevelFragment.Level l = new ListLevelFragment.Level(em.niveau(), em.nomSalle(), indexLevel++);
            lSLevel.add(l);
        }
        listLevelFragment.setLsLevel(lSLevel);
        timeTableFragment.setLsTimeTable(lsTimeTable);

        //Socket de connextion
        studentSocket = new StudentSocket();
        studentSocket.setDatabase(database);
        studentSocket.setObjectName(Constant.MAIN_SOCKET_NAME);
        studentSocket.addListener(this);
        studentSocket.start();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()){
            case R.id.id_item_setting:
            case R.id.id_item_setting1:
                Intent intent = new Intent(this, SettingActivity.class);
                LocalDatabase.Parametre parametre = database.getPrametre();
                intent.putExtra(SettingActivity.HOST_NAME, parametre.hostName());
                intent.putExtra(SettingActivity.PORT_NAME, (int) parametre.portName());
                intent.putExtra(SettingActivity.LEVEL, parametre.level());
                startActivityForResult(intent, R.layout.activity_setting);
                break;
            case R.id.id_item_help:
                showFragment(R.layout.fragment_help);
                return true;
            case R.id.id_item_home:
                showFragment(R.layout.fragment_home);
                break;
            case R.id.id_item_about:
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setMessage(Html.fromHtml(Constant.TEXT_ABOUT, Html.FROM_HTML_MODE_COMPACT));
                builder.setPositiveButton("ok", null);
                builder.show();

                break;
            case R.id.id_item_quit:
                onBackPressed();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        if(requestCode == AppCompatActivity.RESULT_CANCELED) return;
        switch (requestCode){
            case R.layout.activity_setting:
                if(data != null){
                    LocalDatabase.Parametre p = database.getPrametre();
                    p.setPortName(data.getIntExtra(SettingActivity.PORT_NAME, 2222));
                    p.setHostName(data.getStringExtra(SettingActivity.HOST_NAME));
                    p.setLevel(data.getStringExtra(SettingActivity.LEVEL));
                    Log.i("MYDEBUG", "On activity result:\nHostName ="+
                            p.hostName()+"\nPort = "+p.portName()+"\nLevel = "+p.level());
                    database.setParametre(p);
                    studentSocket.setSetting(p);
                    Toast.makeText(this, "Enregistrer", Toast.LENGTH_LONG).show();
                }
                break;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    void showFragment(int id){
        if(!lsUi.empty() && lsUi.lastElement() == id)
            return;
        idCurrentFragment = id;
        FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
        if (id == R.layout.fragment_home)
            fragmentTransaction.replace(ui.centralWidget.getId(), homeWidget);
        else if(id == R.layout.fragment_help)
            fragmentTransaction.replace(ui.centralWidget.getId(), helpWidget);
        else if(id == R.layout.fragment_timetable)
            fragmentTransaction.replace(ui.centralWidget.getId(), timeTableFragment);
        else if(id == R.layout.fragment_list_level)
            fragmentTransaction.replace(ui.centralWidget.getId(), listLevelFragment);
        else
            return;
        fragmentTransaction.commit();
        if(!lsUi.isEmpty())
            lsUi.pop();
        lsUi.push(id);
    }
    void showShedule(){
        showFragment(R.layout.fragment_list_level);
        //showFragment(R.layout.fragment_timetable);
    }

    @Override
    public void onBackPressed() {
        if(idCurrentFragment != R.layout.fragment_home){
            showFragment(R.layout.fragment_home);
            return;
        }
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage(R.string.messageOnQuit);
        //final boolean[] quited = {false};
        builder.setPositiveButton("Oui", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                finish();
            }
        });
        builder.setNegativeButton("Non", null);
        builder.show();
    }
    @Override
    public void timeTableAvailable(EmploiDuTemp em) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                timeTableFragment.timeTableAvailable(em);
            }
        });
    }

    @Override
    public void status(String state) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ui.mainActivityLabelStatus.setText(state);
            }
        });
    }

    @Override
    public void signatureAvailable(String level, String nomSalle, VDate dateDebut, VDate dateFin, byte[] signature) {

    }

    @Override
    protected void onSaveInstanceState(@NonNull Bundle outState) {
        outState.putInt(CURRENT_ID_FRAGMNET_TEXT, idCurrentFragment);
        outState.putInt(LAST_INDEX_TIMETABLE, timeTableFragment.getCurrentIndex());
        super.onSaveInstanceState(outState);
    }
}