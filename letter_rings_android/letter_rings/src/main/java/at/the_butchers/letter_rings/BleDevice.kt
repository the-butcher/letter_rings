package at.the_butchers.letter_rings

import android.Manifest
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattDescriptor
import android.bluetooth.BluetoothProfile
import android.content.Context
import android.os.Handler
import android.os.Looper
import android.util.Log
import androidx.annotation.RequiresPermission
import java.nio.ByteBuffer
import java.util.UUID

class BleDevice(val device: BluetoothDevice, private val side: Side) {

    val address: String? = device.address
    var gattInstance: BluetoothGatt? = null;
    var labelCharacteristic: BluetoothGattCharacteristic? = null
    var modusCharacteristic: BluetoothGattCharacteristic? = null

    @androidx.annotation.RequiresPermission(android.Manifest.permission.BLUETOOTH_CONNECT)
    fun writeLabelValue(value: String) {
        if (gattInstance != null && labelCharacteristic != null) {
            gattInstance!!.writeCharacteristic(labelCharacteristic!!, value.toByteArray(), BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        }
    }

    @androidx.annotation.RequiresPermission(android.Manifest.permission.BLUETOOTH_CONNECT)
    fun writeModusValue(modus: Int) {
        if (gattInstance != null && modusCharacteristic != null) {
            val bytes = ByteBuffer.allocate(Int.SIZE_BYTES).putInt(modus).array()
            bytes.reverse() // significant byte need to be first
            Log.i("BLE", "writing modus value ${bytes.size} ${bytes[0]} ${bytes[3]}")
            gattInstance!!.writeCharacteristic(modusCharacteristic!!, bytes, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun readModusValue() {
        val readResult = gattInstance?.readCharacteristic(modusCharacteristic);
        Log.i("BLE","modus characteristic read (result: ${readResult})") // returns true
    }

    // https://punchthrough.com/android-ble-guide/
    fun BluetoothGattCharacteristic.isIndicatable(): Boolean =
        containsProperty(BluetoothGattCharacteristic.PROPERTY_INDICATE)

    fun BluetoothGattCharacteristic.isNotifiable(): Boolean =
        containsProperty(BluetoothGattCharacteristic.PROPERTY_NOTIFY)

    fun BluetoothGattCharacteristic.containsProperty(property: Int): Boolean =
        properties and property != 0

    @androidx.annotation.RequiresPermission(android.Manifest.permission.BLUETOOTH_CONNECT)
    fun enableNotifications(characteristic: BluetoothGattCharacteristic) {
        val cccdUuid = UUID.fromString(CCC_DESCRIPTOR____UUID)
        val payload = when {
            characteristic.isIndicatable() -> BluetoothGattDescriptor.ENABLE_INDICATION_VALUE
            characteristic.isNotifiable() -> BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
            else -> {
                Log.e("ConnectionManager", "${characteristic.uuid} doesn't support notifications/indications")
                return
            }
        }

        characteristic.getDescriptor(cccdUuid)?.let() { cccDescriptor ->
            if (gattInstance?.setCharacteristicNotification(characteristic, true) == false) {
                Log.e("ConnectionManager", "setCharacteristicNotification failed for ${characteristic.uuid}")
                return
            }
            writeDescriptor(cccDescriptor, payload)
        } ?: Log.e("ConnectionManager", "${characteristic.uuid} doesn't contain the CCC descriptor!")

    }

    @androidx.annotation.RequiresPermission(android.Manifest.permission.BLUETOOTH_CONNECT)
    fun writeDescriptor(descriptor: BluetoothGattDescriptor, payload: ByteArray) {
        gattInstance?.writeDescriptor(descriptor, payload) ?: error("failed to write descriptor due to missing gatt instance")
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun disconnectGatt(context: Context) {
        gattInstance?.close();
        gattInstance?.disconnect();
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun connectGatt(context: Context) {

        val gattCallback = object : BluetoothGattCallback() {

            @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
            override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    Log.d("BLE", "connected to GATT")
                    gatt.discoverServices()
                } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                    Log.d("BLE", "disconnected from GATT")
                    // TODO :: reset connection
                    gattInstance = null;
                    MainActivity.instance.get()?.checkBleState(side)
                }
            }

            override fun onCharacteristicRead(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, value: ByteArray, status: Int) {
                Log.i("BLE", "read characteristic (status: ${status})")
                val uuid = characteristic.uuid
                when (status) {
                    BluetoothGatt.GATT_SUCCESS -> {
                        if (characteristic.uuid.toString() == COMMAND_MODUS_____UUID) {
                            val modus = value[0]
                            MainActivity.instance.get()?.setModus(modus, side)
                            Log.i("BLE", "received modus change (${modus})")
                        } else {
                            Log.i("BLE", "received unknown characteristic change (${characteristic.uuid})")
                        }
                    }
                    BluetoothGatt.GATT_READ_NOT_PERMITTED -> {
                        Log.e("BLE", "read not permitted for $uuid!")
                    }
                    else -> {
                        Log.e("BLE", "characteristic read failed for $uuid, error: $status")
                    }
                }
            }

            @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
            public override fun onCharacteristicChanged(gatt: BluetoothGatt?, characteristic: BluetoothGattCharacteristic) {
                Log.i("BLE", "received characteristics changed event: " + characteristic.uuid)
                if (characteristic.uuid.toString() == COMMAND_MODUS_____UUID) {
                    readModusValue()
//                    val readResult = gattInstance?.readCharacteristic(characteristic);
//                    Log.i("BLE","modus characteristic read (${gattInstance}, ${characteristic}, ${readResult})") // returns true
                }
            }

            @androidx.annotation.RequiresPermission(android.Manifest.permission.BLUETOOTH_CONNECT)
            override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
                if (status == BluetoothGatt.GATT_SUCCESS) {

                    val services = gatt.services
                    Log.d("BLE", "found services: ${services.size}")
                    val service = gatt.getService(UUID.fromString(side.serviceUuid))
                    if (service != null) {

                        Log.d("BLE", "letter-rings service bind (${service.toString()} ${address ?: "none"})")
                        labelCharacteristic = service.getCharacteristic(UUID.fromString(COMMAND_LABEL_____UUID))
                        modusCharacteristic = service.getCharacteristic(UUID.fromString(COMMAND_MODUS_____UUID))

                        modusCharacteristic?.let {

                            enableNotifications(it)

                            // get initial modus value
                            Handler(Looper.getMainLooper()).postDelayed({
                                try {
                                    readModusValue()
                                } catch (e: Exception) {
                                    Log.e("BLE", "failed to read modus ${e.message}")
                                }
                            }, 1000)

                            // read initial value
//                            val modus = it.value[0]
//                            MainActivity.instance.get()?.setModus(modus, side)

                        } ?: Log.i("BLE","modus characteristic not found") // returns true

                        MainActivity.instance.get()?.checkBleState(side)

                    } else {

                        Log.d("BLE", "letter-rings service lost (${service.toString()} ${address ?: "none"})")
                        // TODO :: reset connection
                        gattInstance = null;
                        MainActivity.instance.get()?.checkBleState(side)

                    }

                }

            }

        }

        gattInstance = device.connectGatt(context,false, gattCallback) // init when device connection is requested.

    }

}