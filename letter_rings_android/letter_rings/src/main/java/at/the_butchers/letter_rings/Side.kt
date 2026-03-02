package at.the_butchers.letter_rings

import androidx.annotation.IdRes

enum class Side(
    val serviceUuid: String,
    @IdRes val idBtConn: Int,
    @IdRes val idLlConn: Int,
//    @IdRes val idLlContr: Int, // controls, modus radio buttons, light slider
    @IdRes val idSwConn: Int,
    @IdRes val idTxAddr: Int
//    @IdRes val idRbWords: Int,
//    @IdRes val idRbLabel: Int,
//    @IdRes val idRbFrequ: Int,
//    @IdRes val idRbParty: Int,
//    @IdRes val idRbAccel: Int,
//    @IdRes val idSbLight: Int
) {

    LEFT(
        COMMAND_SERVICE_UUID_L,
        R.id.btConnL,
        R.id.llConnL, // connection switch and device address
//        R.id.llContrL, // radiobuttons and light, should be removable
        R.id.swConnL,
        R.id.txAddrL
//        R.id.radio_wordsL, // common
//        R.id.radio_labelL, // common
//        R.id.radio_frequL, // common
//        R.id.radio_partyL, // common
//        R.id.radio_accelL, // common
//        R.id.sbLightL // common
    ),
    RIGHT(
        COMMAND_SERVICE_UUID_R,
        R.id.btConnR,
        R.id.llConnR,
//        R.id.llContrR,
        R.id.swConnR,
        R.id.txAddrR
//        R.id.radio_wordsR,
//        R.id.radio_labelR,
//        R.id.radio_frequR,
//        R.id.radio_partyR,
//        R.id.radio_accelR,
//        R.id.sbLightR
    )

}