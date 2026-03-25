package at.the_butchers.letter_rings

import androidx.annotation.IdRes

enum class Side(
    val serviceUuid: String,
    @param:IdRes val idBtConn: Int,
    @param:IdRes val idLlConn: Int,
    @param:IdRes val idSwConn: Int,
    @param:IdRes val idTxAddr: Int,
    @param:IdRes val idTxLabel: Int,
    @param:IdRes val idEdLabel: Int
) {

    LEFT(
        COMMAND_SERVICE_UUID_L,
        R.id.btConnL,
        R.id.llConnL, // connection switch and device address
        R.id.swConnL,
        R.id.txAddrL,
        R.id.txLabelL,
        R.id.edLabelL
    ),
    RIGHT(
        COMMAND_SERVICE_UUID_R,
        R.id.btConnR,
        R.id.llConnR,
        R.id.swConnR,
        R.id.txAddrR,
        R.id.txLabelR,
        R.id.edLabelR
    )

}