package at.the_butchers.letter_rings

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.le.BluetoothLeScanner
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanFilter
import android.bluetooth.le.ScanResult
import android.bluetooth.le.ScanSettings
import android.os.Handler
import android.os.Looper
import android.os.ParcelUuid
import android.util.Log
import androidx.annotation.RequiresPermission
import java.util.UUID

class BleScanner(private val bluetoothAdapter: BluetoothAdapter, private val side: Side) {

    private val scanner: BluetoothLeScanner? = bluetoothAdapter.bluetoothLeScanner

    private val handler = Handler(Looper.getMainLooper())

    private var isScanning = false

    private var addressSet: MutableSet<String> = mutableSetOf()

    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    fun startScan() {

        if (!isScanning) {

            Log.d("BLE", "starting BLE scan")
            isScanning = true

            val filters = listOf(
                ScanFilter.Builder()
                    .setServiceUuid(ParcelUuid(UUID.fromString(side.serviceUuid)))
                    .build()
            )

            // Define ScanSettings
            val scanSettings = ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY) // Adjust based on use case
                .build()

            scanner?.startScan(filters, scanSettings, scanCallback)

            // Stop scanning after a timeout (e.g., 10 seconds)
            handler.postDelayed({
                stopScan()
            }, 10000) // 10,000 ms = 10 seconds

        }

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    fun stopScan() {

        if (isScanning) {

            Log.d("BLE", "stopping BLE scan")
            isScanning = false
            scanner?.stopScan(scanCallback)
            addressSet.clear()

        }

    }

    private val scanCallback = object : ScanCallback() {

        @RequiresPermission(allOf = [Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.BLUETOOTH_SCAN])
        override fun onScanResult(callbackType: Int, result: ScanResult) {

            val device = result.device
            if (!addressSet.contains(device.address)) {
                Log.d("BLE", "new device found: ${device.address ?: "no address"}")
                addressSet.add(device.address)
                val bleDevice = BleDevice(device, side)
                MainActivity.instance.get()?.checkBleState(side)
                MainActivity.instance.get()?.setBleDevice(bleDevice, side)

                stopScan()

            }

        }

        override fun onScanFailed(errorCode: Int) {
            // Log.e("BLE", "Scan failed with error code: $errorCode")
        }

    }
}