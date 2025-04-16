import * as UE from 'ue';
import mixin from '../../../mixin';

const AssetPath = '/Game/Project/Blueprints/Input/AC_Input.AC_Input_C';

export interface AC_Input extends UE.Game.Project.Blueprints.Input.AC_Input.AC_Input_C {}

@mixin(AssetPath)
export class AC_Input implements AC_Input {
    //
}
