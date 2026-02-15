package at.the_butchers.letter_rings


import android.Manifest
import android.bluetooth.BluetoothManager
import android.content.pm.PackageManager
import android.content.res.AssetManager
import android.os.Bundle
import android.util.Log
import android.view.View
import android.view.WindowManager
import android.widget.LinearLayout
import android.widget.RadioButton
import android.widget.RadioGroup
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


class MainActivity : AppCompatActivity() {

    companion object {
        var instance: WeakReference<MainActivity?> = WeakReference<MainActivity?>(null)
        private const val PERMISSION_REQUEST_CODE = 123
    }

    var isRecognitionChecked = false

    var catalog: ShazamCatalog? = null

    val bleDeviceInstanceMap: EnumMap<Side, BleDevice?> = EnumMap(Side.entries.associateWith { null })

    @RequiresPermission(allOf=[Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.WAKE_LOCK, Manifest.permission.BLUETOOTH_CONNECT])
    override fun onCreate(savedInstanceState: Bundle?) {

        super.onCreate(savedInstanceState)

        requestPermissions()

        Log.i("MAIN", "main create")

        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        supportActionBar?.hide()
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, 60, systemBars.right, systemBars.bottom)
            insets
        }

        val properties = Properties()
        val assetManager: AssetManager = this.assets
        val inputStream = assetManager.open("shazam.properties")
        properties.load(inputStream)
        val shazamJwt = properties.getProperty("SHAZAM_JWT")
        Log.d("MAIN", "shazamJwt from file ($shazamJwt)")

        val tokenProvider = DeveloperTokenProvider {
            DeveloperToken(shazamJwt)
        }
        catalog = ShazamKit.createShazamCatalog(tokenProvider)

        val recognitionSwitch: Switch = findViewById (R.id.swRecognition)
        recognitionSwitch.setOnCheckedChangeListener { _, isChecked ->
            isRecognitionChecked = isChecked
        }

        setupRadioButtons(Side.LEFT)
        setupRadioButtons(Side.RIGHT)

        setupSeekBar(Side.LEFT)
        setupSeekBar(Side.RIGHT)

        // set up bluetooth and find devices ====================
        val bluetoothManager = getSystemService(BLUETOOTH_SERVICE) as BluetoothManager
        val bluetoothAdapter = bluetoothManager.adapter
        BleScanner(bluetoothAdapter, Side.LEFT).startScan()
        BleScanner(bluetoothAdapter, Side.RIGHT).startScan()

        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun setupRadioButtons(side: Side) {
        setupRadioButton(side, side.idRbWords, MODUS________WORDS)
        setupRadioButton(side, side.idRbLabel, MODUS________LABEL)
        setupRadioButton(side, side.idRbFrequ, MODUS________FREQU)
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun setupRadioButton(side: Side, @IdRes idRb: Int, modus: Int) {
        val rgModus: RadioButton =  findViewById (idRb)
        rgModus.setOnCheckedChangeListener { _, isChecked ->
            Log.d("MODUS", "rgModus checked: $isChecked")
            if (isChecked) {
                bleDeviceInstanceMap[side]?.writeModusValue(modus)
            }
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun setupSeekBar(side: Side) {
        val sbLight: SeekBar =  findViewById (side.idSbLight)
        sbLight.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                Log.i("MAIN", "onStopTrackingTouch (${side.idSbLight})")
                bleDeviceInstanceMap[side]?.writeLightValue(sbLight.progress)
            }
            override fun onStartTrackingTouch(seekBar: SeekBar) {
//                Log.i("MAIN", "onStartTrackingTouch (${side.idSbLight}, ${sbLight.progress})")
            }
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
//                Log.i("MAIN", "onProgressChanged (${side.idSbLight}, ${progress})")
            }
        })
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun setBleDevice(bleDevice: BleDevice, side: Side) {

        bleDeviceInstanceMap[side] = bleDevice

        val bleDeviceLSwitch: Switch =  findViewById (side.idSwConn)
        val txAddrL: TextView =  findViewById (side.idTxAddr)
        bleDeviceLSwitch.isEnabled = true
        txAddrL.visibility = View.VISIBLE
        txAddrL.text = bleDeviceInstanceMap[side]?.address

        val context = this
        bleDeviceLSwitch.setOnCheckedChangeListener  { _, isChecked ->
            if (isChecked) {
                bleDeviceInstanceMap[side]?.connectGatt(context)

            } else {
                bleDeviceInstanceMap[side]?.disconnectGatt(context)
            }
        }

    }

    fun setModus(modus: Byte, side: Side) {

        Log.d("BLE", "set modus ($modus, $side)")
        if (modus.toInt() == MODUS________WORDS) {
            val rgModusWords: RadioButton =  findViewById (side.idRbWords)
            Log.d("BLE", "rgModusWords will be checked on ui-thread")
            this@MainActivity.runOnUiThread { rgModusWords.setChecked(true) }
        } else if (modus.toInt() == MODUS________LABEL) {
            val rgModusLabel: RadioButton =  findViewById (side.idRbLabel)
            Log.d("BLE", "rgModusLabel will be checked on ui-thread")
            this@MainActivity.runOnUiThread { rgModusLabel.setChecked(true) }
        } else if (modus.toInt() == MODUS________FREQU) {
            val rgModusFrequ: RadioButton =  findViewById (side.idRbFrequ)
            Log.d("BLE", "rgModusFrequ will be checked on ui-thread")
            this@MainActivity.runOnUiThread { rgModusFrequ.setChecked(true) }
        }

    }

    fun setLight(light: Byte, side: Side) {

        val sbLight: SeekBar =  findViewById (side.idSbLight)
        Log.d("BLE", "sbLight will be changed on ui-thread")
        this@MainActivity.runOnUiThread { sbLight.setProgress((light.toInt())) }

    }

    fun checkBleState(side: Side) {

        Log.d("BLE", "unchecking BLE connection")
        val bleDeviceSwitch: Switch =  findViewById (side.idSwConn)
        val llContr: LinearLayout =  findViewById ( side.idLlContr)
        if (bleDeviceInstanceMap[side]?.gattInstance == null) {
            Log.d("BLE", "gattL is null")
            if (bleDeviceSwitch.isChecked) {
                Log.d("BLE", "switch is checked, will uncheck on ui-thread")
                this@MainActivity.runOnUiThread { bleDeviceSwitch.toggle() }
            }
            if (llContr.visibility == View.VISIBLE) { // set the radio groups visibility to gone if visibility
                Log.d("BLE", "llContr is VISIBLE, will set to GONE")
                this@MainActivity.runOnUiThread { llContr.setVisibility(View.GONE) }
            }
        } else { // set the radio groups visibility to visible if gone
            if (llContr.visibility == View.GONE) { // show radio group if gone
                Log.d("BLE", "llContr is GONE, will set to VISIBLE")
                this@MainActivity.runOnUiThread { llContr.setVisibility(View.VISIBLE) }
            }
        }

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun updateTxRecognition(title: String, artist: String, valid: Boolean) {

        Log.i("MAIN", "update tx (title: $title) (artist: $artist)")

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

    override fun onResume() {

        super.onResume()

        Log.i("MAIN", "main resume")

        //keep current activity instance and use to perform any UI related task on work completion
        instance = WeakReference(this)
        WorkHandler.scheduleWork()

    }

    override fun onStop() {

        Log.i("MAIN", "main stop")

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
            Toast.makeText(this, "All permissions already granted", Toast.LENGTH_SHORT).show()
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

