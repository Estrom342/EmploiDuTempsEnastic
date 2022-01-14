package td.enastic.emploienastic;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import td.enastic.emploienastic.databinding.ActivitySettingBinding;

public class SettingActivity extends AppCompatActivity {

    public static final String HOST_NAME = "HOST_NAME";
    public static final String PORT_NAME = "POT_NAME";
    public static final String LEVEL = "LEVEL_SETTING";
    ActivitySettingBinding ui;

    @SuppressLint("SetTextI18n")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ui = ActivitySettingBinding.inflate(getLayoutInflater());
        ui.settingActivityIdBtnCancel.setOnClickListener(v ->{this.onBackPressed();});
        ui.settingActivityIdBtnSave.setOnClickListener(v->{this.saveSetting();});
        ui.settingActivityIdHostName.setText(getIntent().getStringExtra(SettingActivity.HOST_NAME));
        ui.settingActivityIdPortName.setText(getIntent().getIntExtra(SettingActivity.PORT_NAME, 2222)+"");
        ui.settingActivityIdClassName.setText(getIntent().getStringExtra(SettingActivity.LEVEL));
        setContentView(ui.getRoot());
    }

    private void saveSetting(){
        Intent intent = getIntent();
        intent.putExtra(HOST_NAME, ui.settingActivityIdHostName.getText().toString());
        intent.putExtra(LEVEL, ui.settingActivityIdClassName.getText().toString());

        try {
            intent.putExtra(PORT_NAME, Integer.valueOf(ui.settingActivityIdPortName.getText().toString()));
        } catch (NumberFormatException e) {
            e.printStackTrace();
        }
        setResult(R.layout.activity_setting, intent);
        finish();
    }
}