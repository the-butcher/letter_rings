package at.the_butchers.letter_rings

import androidx.annotation.IdRes

enum class Side(val serviceUuid: String, @IdRes val idSwConn: Int, @IdRes val idTxAddr: Int, @IdRes val idLlContr: Int, @IdRes val idRbWords: Int, @IdRes val idRbLabel: Int, @IdRes val idRbFrequ: Int, @IdRes val idSbLight: Int) {

    LEFT(COMMAND_SERVICE_UUID_L, R.id.swConnL, R.id.txAddrL, R.id.llContrL, R.id.radio_wordsL, R.id.radio_labelL, R.id.radio_frequL, R.id.sbLightL),
    RIGHT(COMMAND_SERVICE_UUID_R, R.id.swConnR, R.id.txAddrR, R.id.llContrR, R.id.radio_wordsR, R.id.radio_labelR, R.id.radio_frequR, R.id.sbLightR)

}