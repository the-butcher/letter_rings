package at.the_butchers.letter_rings


import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothManager
import android.content.pm.PackageManager
import android.content.res.AssetManager
import android.os.Bundle
import android.util.Log
import android.view.View
import android.view.WindowManager
import android.widget.Button
import android.widget.LinearLayout
import android.widget.RadioButton
import android.widget.SeekBar
import android.widget.Switch
import android.widget.TextView
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.annotation.IdRes
import androidx.annotation.RequiresPermission
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.shazam.shazamkit.DeveloperToken
import com.shazam.shazamkit.DeveloperTokenProvider
import com.shazam.shazamkit.ShazamCatalog
import com.shazam.shazamkit.ShazamKit
import java.lang.ref.WeakReference
import java.util.EnumMap
import java.util.Properties
import androidx.core.view.isGone
import androidx.core.view.isVisible


class MainActivity : AppCompatActivity() {

    companion object {
        var instance: WeakReference<MainActivity?> = WeakReference<MainActivity?>(null)
        private const val PERMISSION_REQUEST_CODE = 123
    }

    var isRecognitionChecked = false

    var catalog: ShazamCatalog? = null

    val bleDeviceInstanceMap: EnumMap<Side, BleDevice?> = EnumMap(Side.entries.associateWith { null })

    @SuppressLint("UseSwitchCompatOrMaterialCode")
    @RequiresPermission(allOf=[Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.WAKE_LOCK, Manifest.permission.BLUETOOTH_CONNECT])
    override fun onCreate(savedInstanceState: Bundle?) {

        super.onCreate(savedInstanceState)

        requestPermissions()

        Log.i(LOG_TAG_MAIN, "main create")

        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        supportActionBar?.hide()
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, 80, systemBars.right, systemBars.bottom)
            insets
        }

        val properties = Properties()
        val assetManager: AssetManager = this.assets
        val inputStream = assetManager.open("shazam.properties")
        properties.load(inputStream)
        val shazamJwt = properties.getProperty("SHAZAM_JWT")
        Log.d(LOG_TAG_MAIN, "shazamJwt from file ($shazamJwt)")

        val tokenProvider = DeveloperTokenProvider {
            DeveloperToken(shazamJwt)
        }
        catalog = ShazamKit.createShazamCatalog(tokenProvider)

        val recognitionSwitch: Switch = findViewById (R.id.swRecognition)
        val btRecognizeNow: Button =  findViewById ( R.id.btRecognition)
        recognitionSwitch.setOnCheckedChangeListener { _, isChecked ->
            isRecognitionChecked = isChecked
            if (isRecognitionChecked != btRecognizeNow.isEnabled) { // set the radio groups visibility to gone if visibility
                Log.d(LOG_TAG_MAIN, "btRecognizeNow will change enabled")
                this@MainActivity.runOnUiThread { btRecognizeNow.setEnabled(isRecognitionChecked) }
            }
        }
        btRecognizeNow.setOnClickListener  {
            Log.i(LOG_TAG_MAIN, "recognize now button clicked")
            WorkHandler.scheduleShazamWork(1)
        }

        setupRadioButtons()
        setupLightBar()
        setupCoeffBar()
        setupConnButton(Side.LEFT)
        setupConnButton(Side.RIGHT)

        // set up bluetooth and find devices ====================
        val bluetoothManager = getSystemService(BLUETOOTH_SERVICE) as BluetoothManager
        val bluetoothAdapter = bluetoothManager.adapter
        BleScanner(bluetoothAdapter, Side.LEFT).startScan()
        BleScanner(bluetoothAdapter, Side.RIGHT).startScan()

        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    fun setupConnButton(side: Side) {

        Log.i(LOG_TAG_BLUE, "set up conn button (${side})")

        val btConn: Button =  findViewById ( side.idBtConn)
        btConn.setOnClickListener  {
            Log.i(LOG_TAG_BLUE, "conn button clicked (${side})")
            val bluetoothManager = getSystemService(BLUETOOTH_SERVICE) as BluetoothManager
            val bluetoothAdapter = bluetoothManager.adapter
            BleScanner(bluetoothAdapter, side).startScan()
        }

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun setupRadioButtons() {
        setupRadioButton( R.id.radio_words, MODUS________WORDS)
        setupRadioButton( R.id.radio_label, MODUS________LABEL)
        setupRadioButton( R.id.radio_frequ, MODUS________FREQU)
        setupRadioButton( R.id.radio_break, MODUS________BREAK)
        setupRadioButton( R.id.radio_party, MODUS________PARTY)
        setupRadioButton( R.id.radio_accel, MODUS________ACCEL)
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun setupRadioButton(@IdRes idRb: Int, modus: Int) {
        val rgModus: RadioButton =  findViewById (idRb)
        rgModus.setOnCheckedChangeListener { _, isChecked ->
            Log.d(LOG_TAG_MAIN, "rgModus checked: $isChecked")
            if (isChecked) {
                bleDeviceInstanceMap[Side.LEFT]?.writeModusValue(modus)
                bleDeviceInstanceMap[Side.RIGHT]?.writeModusValue(modus)
            }
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun setupLightBar() {
        val sbLight: SeekBar =  findViewById (R.id.sbLight)
        sbLight.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                Log.i(LOG_TAG_MAIN, "stop-tracking-touch (${R.id.sbLight})")
                bleDeviceInstanceMap[Side.LEFT]?.writeLightValue(sbLight.progress)
                bleDeviceInstanceMap[Side.RIGHT]?.writeLightValue(sbLight.progress)
            }
            override fun onStartTrackingTouch(seekBar: SeekBar) {
//                Log.i(LOG_TAG_MAIN, "onStartTrackingTouch (${side.idSbLight}, ${sbLight.progress})")
            }
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
//                Log.i(LOG_TAG_MAIN, "onProgressChanged (${side.idSbLight}, ${progress})")
            }
        })
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun setupCoeffBar() {
        val sbCoeff: SeekBar =  findViewById (R.id.sbCoeff)
        sbCoeff.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                Log.i(LOG_TAG_MAIN, "stop-tracking-touch (${R.id.sbCoeff})")
                bleDeviceInstanceMap[Side.LEFT]?.writeCoeffValue(sbCoeff.progress)
                bleDeviceInstanceMap[Side.RIGHT]?.writeCoeffValue(sbCoeff.progress)
            }
            override fun onStartTrackingTouch(seekBar: SeekBar) {
//                Log.i(LOG_TAG_MAIN, "onStartTrackingTouch (${side.sbCoeff}, ${sbCoeff.progress})")
            }
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
//                Log.i(LOG_TAG_MAIN, "onProgressChanged (${side.sbCoeff}, ${progress})")
            }
        })
    }

    @SuppressLint("UseSwitchCompatOrMaterialCode")
    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun setBleDevice(bleDevice: BleDevice, side: Side) {

        bleDeviceInstanceMap[side] = bleDevice

        Log.i(LOG_TAG_BLUE, "set ble-device (${side}, ${bleDevice.address})")

        val bleDeviceSwitch: Switch =  findViewById (side.idSwConn)
        val txAddr: TextView =  findViewById (side.idTxAddr)
        this@MainActivity.runOnUiThread { txAddr.setText(bleDeviceInstanceMap[side]?.address) }

        val llConn: LinearLayout =  findViewById ( side.idLlConn)
        this@MainActivity.runOnUiThread { llConn.setVisibility(View.VISIBLE) }

        val btConn: Button =  findViewById ( side.idBtConn)
        this@MainActivity.runOnUiThread { btConn.setVisibility(View.GONE) }

        val context = this
        bleDeviceSwitch.setOnCheckedChangeListener  { _, isChecked ->
            if (isChecked) {
                bleDeviceInstanceMap[side]?.connectGatt(context)
            } else {
                bleDeviceInstanceMap[side]?.disconnectGatt()
            }
        }

    }

    fun removeBleDevice(side: Side) {

        bleDeviceInstanceMap[side] = null

        val llConn: LinearLayout =  findViewById ( side.idLlConn)
        this@MainActivity.runOnUiThread { llConn.setVisibility(View.GONE) }

        val btConn: Button =  findViewById ( side.idBtConn)
        this@MainActivity.runOnUiThread { btConn.setVisibility(View.VISIBLE) }

    }

    fun setModus(modus: Byte) {

        // TODO :: warn if settings are inconsistent

        Log.d(LOG_TAG_BLUE, "set modus ($modus)")
        if (modus.toInt() == MODUS________WORDS) {
            val rgModusWords: RadioButton =  findViewById (R.id.radio_words)
            Log.d(LOG_TAG_BLUE, "rgModusWords will be checked on ui-thread")
            this@MainActivity.runOnUiThread { rgModusWords.setChecked(true) }
        } else if (modus.toInt() == MODUS________LABEL) {
            val rgModusLabel: RadioButton =  findViewById (R.id.radio_label)
            Log.d(LOG_TAG_BLUE, "rgModusLabel will be checked on ui-thread")
            this@MainActivity.runOnUiThread { rgModusLabel.setChecked(true) }
        } else if (modus.toInt() == MODUS________FREQU) {
            val rgModusFrequ: RadioButton =  findViewById (R.id.radio_frequ)
            Log.d(LOG_TAG_BLUE, "rgModusFrequ will be checked on ui-thread")
            this@MainActivity.runOnUiThread { rgModusFrequ.setChecked(true) }
        } else if (modus.toInt() == MODUS________BREAK) {
            val rgModusBreak: RadioButton =  findViewById (R.id.radio_break)
            Log.d(LOG_TAG_BLUE, "rgModusBreak will be checked on ui-thread")
            this@MainActivity.runOnUiThread { rgModusBreak.setChecked(true) }
        } else if (modus.toInt() == MODUS________PARTY) {
            val rgModusParty: RadioButton =  findViewById (R.id.radio_party)
            Log.d(LOG_TAG_BLUE, "rgModusParty will be checked on ui-thread")
            this@MainActivity.runOnUiThread { rgModusParty.setChecked(true) }
        } else if (modus.toInt() == MODUS________ACCEL) {
            val rgModusAccel: RadioButton =  findViewById (R.id.radio_accel)
            Log.d(LOG_TAG_BLUE, "rgModusAccel will be checked on ui-thread")
            this@MainActivity.runOnUiThread { rgModusAccel.setChecked(true) }
        }

    }

    fun setLight(light: Byte) {

        // TODO :: warn if settings are inconsistent

        val sbLight: SeekBar =  findViewById (R.id.sbLight)
        Log.d(LOG_TAG_BLUE, "sbLight will be changed on ui-thread")
        this@MainActivity.runOnUiThread { sbLight.setProgress((light.toInt())) }

    }

    fun setCoeff(coeff: Byte) {

        // TODO :: warn if settings are inconsistent

        val sbCoeff: SeekBar =  findViewById (R.id.sbCoeff)
        Log.d(LOG_TAG_BLUE, "sbCoeff will be changed on ui-thread")
        this@MainActivity.runOnUiThread { sbCoeff.setProgress((coeff.toInt())) }

    }


    @SuppressLint("UseSwitchCompatOrMaterialCode")
    fun checkBleState(side: Side) {

        Log.i(LOG_TAG_BLUE, "check ble-state (side: ${side}, gatt: ${bleDeviceInstanceMap[side]?.gattInstance})")

        val bleDeviceSwitch: Switch =  findViewById (side.idSwConn)
        val llContr: LinearLayout =  findViewById ( R.id.llContr)
        if (bleDeviceInstanceMap[Side.LEFT]?.gattInstance == null && bleDeviceInstanceMap[Side.RIGHT]?.gattInstance == null) {

            // none of LEFT or RIGHT is connected - hide controls
            if (llContr.isVisible) { // set the radio groups visibility to gone if visibility
                Log.d(LOG_TAG_BLUE, "llContr is VISIBLE, will set to GONE")
                this@MainActivity.runOnUiThread { llContr.setVisibility(View.GONE) }
            }

        } else {

            // at least one of LEFT or RIGHT are connected - show controls
            if (llContr.isGone) { // show radio group if gone
                Log.d(LOG_TAG_BLUE, "llContr is GONE, will set to VISIBLE")
                this@MainActivity.runOnUiThread { llContr.setVisibility(View.VISIBLE) }
            }

            if (bleDeviceInstanceMap[side]?.gattInstance == null) {
                Log.d(LOG_TAG_BLUE, "gattL is null")
                if (bleDeviceSwitch.isChecked) {
                    Log.d(LOG_TAG_BLUE, "switch is checked, will uncheck on ui-thread")
                    this@MainActivity.runOnUiThread { bleDeviceSwitch.toggle() }
                }

            } else { // set the radio groups visibility to visible if gone

            }

        }

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun updateLabels(title: String, artist: String, valid: Boolean) {

        Log.i(LOG_TAG_MAIN, "update tx (textL: $title) (textR: $artist)")

        val txTitle = findViewById<View?>(R.id.txTitle) as TextView
        val txArtist = findViewById<View?>(R.id.txArtist) as TextView
        txTitle.text = title
        txArtist.text = artist

        // pass to any connected device
        if (valid) {
            bleDeviceInstanceMap[Side.LEFT]?.writeLabelValue(title)
            bleDeviceInstanceMap[Side.RIGHT]?.writeLabelValue(artist)
        } else {
            // maybe the devices should go to MODUS________FREQU after not receiving a title for a while
        }

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun updateWords(wordL: String, wordR: String) {

        Log.i(LOG_TAG_MAIN, "update words (wordL: $wordL, wordR: $wordR)")

        val txWordL = findViewById<View?>(R.id.txWordL) as TextView
        val txWordR = findViewById<View?>(R.id.txWordR) as TextView
        txWordL.text = wordL
        txWordR.text = wordR

        bleDeviceInstanceMap[Side.LEFT]?.writeWordValue(wordL)
        bleDeviceInstanceMap[Side.RIGHT]?.writeWordValue(wordR)

    }

    override fun onResume() {

        super.onResume()

        Log.i(LOG_TAG_MAIN, "main resume")

        //keep current activity instance and use to perform any UI related task on work completion
        instance = WeakReference(this)
        WorkHandler.scheduleShazamWork(60)
        WorkHandler.scheduleWordPairWork()

    }

    override fun onStop() {

        Log.i(LOG_TAG_MAIN, "main stop")

//        WorkHandler.cancelPreviousWork()
//        instance = WeakReference(null)

        super.onStop()

    }

    // Function to check and request necessary permissions
    private fun requestPermissions() {

        // List of permissions the app may need
        val permissions = arrayOf(
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.BLUETOOTH_SCAN,
            Manifest.permission.BLUETOOTH_CONNECT,
            Manifest.permission.INTERNET,
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.WAKE_LOCK
        )

        // Filter out the permissions that are not yet granted
        val permissionsToRequest = permissions.filter { permission ->
            ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED
        }

        // If there are permissions that need to be requested, ask the user for them
        if (permissionsToRequest.isNotEmpty()) {
            ActivityCompat.requestPermissions(
                this,
                permissionsToRequest.toTypedArray(), // Convert list to array
                PERMISSION_REQUEST_CODE // Pass the request code
            )
        } else {
            // All permissions are already granted
            Toast.makeText(this, "all permissions already granted", Toast.LENGTH_SHORT).show()
        }
    }

    // Callback function that handles the result of the permission request
    @Deprecated("This method has been deprecated in favor of using the Activity Result API\n      which brings increased type safety via an {@link ActivityResultContract} and the prebuilt\n      contracts for common intents available in\n      {@link androidx.activity.result.contract.ActivityResultContracts}, provides hooks for\n      testing, and allow receiving results in separate, testable classes independent from your\n      activity. Use\n      {@link #registerForActivityResult(ActivityResultContract, ActivityResultCallback)} passing\n      in a {@link RequestMultiplePermissions} object for the {@link ActivityResultContract} and\n      handling the result in the {@link ActivityResultCallback#onActivityResult(Object) callback}.")
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)

        if (requestCode == PERMISSION_REQUEST_CODE) {

            // Combine permissions with their corresponding grant results
            val deniedPermissions = permissions.zip(grantResults.toTypedArray())
                .filter { it.second != PackageManager.PERMISSION_GRANTED } // Filter out the denied ones
                .map { it.first } // Get the permission names

            if (deniedPermissions.isEmpty()) {
                // All permissions granted
                Toast.makeText(this, "All permissions granted", Toast.LENGTH_SHORT).show()
            } else {
                // Some permissions were denied, show them in a Toast
                Toast.makeText(this, "Permissions denied: $deniedPermissions", Toast.LENGTH_LONG).show()
            }
        }
    }

}

