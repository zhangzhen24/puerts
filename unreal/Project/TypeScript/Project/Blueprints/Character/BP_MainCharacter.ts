import * as UE from 'ue';
import mixin from '../../../mixin';
import { log } from 'console';

const AssetPath = '/Game/Project/Blueprints/Character/BP_MainCharacter.BP_MainCharacter_C';
export interface BP_MainCharacter extends UE.Game.Project.Blueprints.Character.BP_MainCharacter.BP_MainCharacter_C {}

/**
 * The player character class for the game.
 */
@mixin(AssetPath, true)
export class BP_MainCharacter implements BP_MainCharacter {
    ReceiveBeginPlay(): void {
        console.warn(`[Puerts] BP_MainCharacter:ReceiveBeginPlay`);
    }
}
