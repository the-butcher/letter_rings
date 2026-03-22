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
    var gattInstance: BluetoothGatt? = null
    var labelCharacteristic: BluetoothGattCharacteristic? = null // set label
    var wordCharacteristic: BluetoothGattCharacteristic? = null // set word
    var modusCharacteristic: BluetoothGattCharacteristic? = null // get/set modus
    var lightCharacteristic: BluetoothGattCharacteristic? = null // get/set light
    var coefPCharacteristic: BluetoothGattCharacteristic? = null // get/set coefficient for pairing
    var coefGCharacteristic: BluetoothGattCharacteristic? = null // get/set coefficient for gestures

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun writeLabelValue(value: String) {
        if (gattInstance != null && labelCharacteristic != null) {
            gattInstance!!.writeCharacteristic(labelCharacteristic!!, value.toByteArray(), BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun writeWordValue(value: String) {
        if (gattInstance != null && wordCharacteristic != null) {
            gattInstance!!.writeCharacteristic(wordCharacteristic!!, value.toByteArray(), BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun writeModusValue(modus: Int) {
        if (gattInstance != null && modusCharacteristic != null) {
            val bytes = ByteBuffer.allocate(Int.SIZE_BYTES).putInt(modus).array()
            bytes.reverse() // significant byte need to be first
            Log.i(LOG_TAG_BLUE, "writing modus value ${bytes.size} ${bytes[0]} ${bytes[3]}")
            gattInstance!!.writeCharacteristic(modusCharacteristic!!, bytes, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun writeLightValue(light: Int) {
        if (gattInstance != null && lightCharacteristic != null) {
            val bytes = ByteBuffer.allocate(Int.SIZE_BYTES).putInt(light).array()
            bytes.reverse() // significant byte need to be first
            Log.i(LOG_TAG_BLUE, "writing light value ${bytes.size} ${bytes[0]} ${bytes[3]}")
            gattInstance!!.writeCharacteristic(lightCharacteristic!!, bytes, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun writeCoefPValue(coefP: Int) {
        if (gattInstance != null && coefPCharacteristic != null) {
            val bytes = ByteBuffer.allocate(Int.SIZE_BYTES).putInt(coefP).array()
            bytes.reverse() // significant byte need to be first
            Log.i(LOG_TAG_BLUE, "writing coefP value ${bytes.size} ${bytes[0]} ${bytes[3]}")
            gattInstance!!.writeCharacteristic(coefPCharacteristic!!, bytes, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun writeCoefGValue(coefG: Int) {
        if (gattInstance != null && coefGCharacteristic != null) {
            val bytes = ByteBuffer.allocate(Int.SIZE_BYTES).putInt(coefG).array()
            bytes.reverse() // significant byte need to be first
            Log.i(LOG_TAG_BLUE, "writing coefG value ${bytes.size} ${bytes[0]} ${bytes[3]}")
            gattInstance!!.writeCharacteristic(coefGCharacteristic!!, bytes, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun readModusValue() {
        val readResult = gattInstance?.readCharacteristic(modusCharacteristic)
        Log.i(LOG_TAG_BLUE,"read-characteristic-modus (result: ${readResult})") // returns true
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun readLightValue() {
        val readResult = gattInstance?.readCharacteristic(lightCharacteristic)
        Log.i(LOG_TAG_BLUE,"read-characteristic-light (result: ${readResult})") // returns true
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun readCoefPValue() {
        val readResult = gattInstance?.readCharacteristic(coefPCharacteristic)
        Log.i(LOG_TAG_BLUE,"read-characteristic-coef-p (result: ${readResult})") // returns true
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun readCoefGValue() {
        val readResult = gattInstance?.readCharacteristic(coefGCharacteristic)
        Log.i(LOG_TAG_BLUE,"read-characteristic-coef-g (result: ${readResult})") // returns true
    }

    // https://punchthrough.com/android-ble-guide/
    fun BluetoothGattCharacteristic.isIndicatable(): Boolean =
        containsProperty(BluetoothGattCharacteristic.PROPERTY_INDICATE)

    fun BluetoothGattCharacteristic.isNotifiable(): Boolean =
        containsProperty(BluetoothGattCharacteristic.PROPERTY_NOTIFY)

    fun BluetoothGattCharacteristic.containsProperty(property: Int): Boolean =
        properties and property != 0

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun enableNotifications(characteristic: BluetoothGattCharacteristic) {
        val cccdUuid = UUID.fromString(CCC_DESCRIPTOR____UUID)
        val payload = when {
            characteristic.isIndicatable() -> BluetoothGattDescriptor.ENABLE_INDICATION_VALUE
            characteristic.isNotifiable() -> BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
            else -> {
                Log.e(LOG_TAG_BLUE, "${characteristic.uuid} doesn't support notifications/indications")
                return
            }
        }

        characteristic.getDescriptor(cccdUuid)?.let { cccDescriptor ->
            if (gattInstance?.setCharacteristicNotification(characteristic, true) == false) {
                Log.e(LOG_TAG_BLUE, "setCharacteristicNotification failed for ${characteristic.uuid}")
                return
            }
            writeDescriptor(cccDescriptor, payload)
        } ?: Log.e(LOG_TAG_BLUE, "${characteristic.uuid} doesn't contain the CCC descriptor!")

    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun writeDescriptor(descriptor: BluetoothGattDescriptor, payload: ByteArray) {
        gattInstance?.writeDescriptor(descriptor, payload) ?: error("failed to write descriptor due to missing gatt instance")
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun disconnectGatt() {
        gattInstance?.close()
        gattInstance?.disconnect()
        gattInstance = null
        Log.i(LOG_TAG_BLUE, "disconnect-gatt (side: ${side})")
        MainActivity.instance.get()?.checkBleState(side)
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    fun connectGatt(context: Context) {

        val gattCallback = object : BluetoothGattCallback() {

            @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
            override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    Log.i(LOG_TAG_BLUE, "connection-state-change (side: ${side}, STATE_CONNECTED)")
                    gatt.discoverServices()
                } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                    Log.i(LOG_TAG_BLUE, "connection-state-change (side: ${side}, STATE_DISCONNECTED)")
                    MainActivity.instance.get()?.removeBleDevice(side)
                    MainActivity.instance.get()?.checkBleState(side)
                }
            }

            override fun onCharacteristicRead(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, value: ByteArray, status: Int) {
//                Log.i(LOG_TAG_BLUE, "characteristic-read (side: ${side}, status: ${status})")
//                val uuid = characteristic.uuid
                when (status) {
                    BluetoothGatt.GATT_SUCCESS -> {
                        if (characteristic.uuid.toString() == COMMAND_MODUS_____UUID) {
                            val modus = value[0]
                            Log.i(LOG_TAG_BLUE, "characteristic-read (side: ${side}, modus: ${modus})")
                            MainActivity.instance.get()?.setModus(modus)
                        } else if (characteristic.uuid.toString() == COMMAND_LIGHT_____UUID) {
                            val light = value[0]
                            Log.i(LOG_TAG_BLUE, "characteristic-read (side: ${side}, light: ${light})")
                            MainActivity.instance.get()?.setLight(light)
                        } else if (characteristic.uuid.toString() == COMMAND_COEFP_____UUID) {
                            val coefP = value[0]
                            Log.i(LOG_TAG_BLUE, "characteristic-read (side: ${side}, coefP: ${coefP})")
                            MainActivity.instance.get()?.setCoefP(coefP)
                        } else if (characteristic.uuid.toString() == COMMAND_COEFG_____UUID) {
                            val coefG = value[0]
                            Log.i(LOG_TAG_BLUE, "characteristic-read (side: ${side}, coefG: ${coefG})")
                            MainActivity.instance.get()?.setCoefG(coefG)
                        } else {
                            Log.w(LOG_TAG_BLUE, "characteristic-read (side: ${side}, unknown characteristic)")
                        }
                    }
                    BluetoothGatt.GATT_READ_NOT_PERMITTED -> {
                        Log.e(LOG_TAG_BLUE, "characteristic-read (side: ${side}, GATT_READ_NOT_PERMITTED)")
                    }
                    else -> {
                        Log.e(LOG_TAG_BLUE, "characteristic-read (side: ${side}, error: $status))")
                    }
                }
            }

            @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
            override fun onCharacteristicChanged(gatt: BluetoothGatt?, characteristic: BluetoothGattCharacteristic) {
                Log.i(LOG_TAG_BLUE, "characteristic-change (side: ${side}, uuid: ${characteristic.uuid})")
                if (characteristic.uuid.toString() == COMMAND_MODUS_____UUID) {
                    Log.i(LOG_TAG_BLUE, "trigger read-characteristic (side: ${side}, modus)")
                    readModusValue()
                } else if (characteristic.uuid.toString() == COMMAND_LIGHT_____UUID) {
                    Log.i(LOG_TAG_BLUE, "trigger read-characteristic (side: ${side}, light)")
                    readLightValue()
                } else if (characteristic.uuid.toString() == COMMAND_COEFP_____UUID) {
                    Log.i(LOG_TAG_BLUE, "trigger read-characteristic (side: ${side}, coefP)")
                    readCoefPValue()
                } else if (characteristic.uuid.toString() == COMMAND_COEFG_____UUID) {
                    Log.i(LOG_TAG_BLUE, "trigger read-characteristic (side: ${side}, coefG)")
                    readCoefGValue()
                }
            }

            @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
            override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {

                if (status == BluetoothGatt.GATT_SUCCESS) {

                    val services = gatt.services
                    Log.d(LOG_TAG_BLUE, "found services: ${services.size}")
                    val service = gatt.getService(UUID.fromString(side.serviceUuid))
                    if (service != null) {

                        labelCharacteristic = service.getCharacteristic(UUID.fromString(COMMAND_LABEL_____UUID))
                        wordCharacteristic = service.getCharacteristic(UUID.fromString(COMMAND_WORD______UUID))
                        modusCharacteristic = service.getCharacteristic(UUID.fromString(COMMAND_MODUS_____UUID))
                        lightCharacteristic = service.getCharacteristic(UUID.fromString(COMMAND_LIGHT_____UUID))
                        coefPCharacteristic = service.getCharacteristic(UUID.fromString(COMMAND_COEFP_____UUID))
                        coefGCharacteristic = service.getCharacteristic(UUID.fromString(COMMAND_COEFG_____UUID))

                        modusCharacteristic?.let {

                            Handler(Looper.getMainLooper()).postDelayed({

                                enableNotifications(modusCharacteristic!!)

                            }, 500)

                            Handler(Looper.getMainLooper()).postDelayed({
                                // get initial modus value
                                try {
                                    readModusValue()
                                } catch (e: Exception) {
                                    Log.e(LOG_TAG_BLUE, "failed to read modus ${e.message}")
                                }

                            }, 750)

                        } ?: Log.i(LOG_TAG_BLUE,"modus characteristic not found") // returns true

                        lightCharacteristic?.let {

                            Handler(Looper.getMainLooper()).postDelayed({

                                enableNotifications(lightCharacteristic!!)

                            }, 1000)

                            Handler(Looper.getMainLooper()).postDelayed({

                                // get initial modus value
                                try {
                                    readLightValue()
                                } catch (e: Exception) {
                                    Log.e(LOG_TAG_BLUE, "failed to read light ${e.message}")
                                }

                            }, 1250)

                        } ?: Log.i(LOG_TAG_BLUE,"light characteristic not found") // returns true

                        coefPCharacteristic?.let {

                            Handler(Looper.getMainLooper()).postDelayed({

                                enableNotifications(coefPCharacteristic!!)

                            }, 1500)

                            Handler(Looper.getMainLooper()).postDelayed({

                                // get initial coefP value
                                try {
                                    readCoefPValue()
                                } catch (e: Exception) {
                                    Log.e(LOG_TAG_BLUE, "failed to read coefP ${e.message}")
                                }

                            }, 1750)

                        } ?: Log.i(LOG_TAG_BLUE,"coefP characteristic not found") // returns true

                        coefGCharacteristic?.let {

                            Handler(Looper.getMainLooper()).postDelayed({

                                enableNotifications(coefGCharacteristic!!)

                            }, 2000)

                            Handler(Looper.getMainLooper()).postDelayed({

                                // get initial coefG value
                                try {
                                    readCoefGValue()
                                } catch (e: Exception) {
                                    Log.e(LOG_TAG_BLUE, "failed to read coefG ${e.message}")
                                }

                            }, 2250)

                        } ?: Log.i(LOG_TAG_BLUE,"coefG characteristic not found") // returns true

                        Log.i(LOG_TAG_BLUE, "services-discovered (side: ${side})")
                        MainActivity.instance.get()?.checkBleState(side)

                    } else {

                        Log.i(LOG_TAG_BLUE, "services-lost (side: ${side})")
                        // TODO :: reset connection
                        gattInstance = null
                        MainActivity.instance.get()?.checkBleState(side)

                    }

                }

            }

        }

        gattInstance = device.connectGatt(context,false, gattCallback) // init when device connection is requested.

    }

}