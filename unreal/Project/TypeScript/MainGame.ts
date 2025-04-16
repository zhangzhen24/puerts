import * as UE from 'ue';
import { argv } from 'puerts';

console.warn(`[Puerts] MainGame.ts 启动！`);

/* 获取游戏实例对象并执行类型断言
 * @param gameInstance - 通过参数管理器获取的PuertsGasGameInstance实例对象
 * @type {UE.PuertsGasGameInstance} - 显式类型断言确保符合游戏实例接口规范 */
const GameInstance = argv.getByName('GameInstance') as UE.HikeGameInstance;

/* 绑定FCall回调函数
 * @param FunctionName - 需要动态调用的方法名称字符串
 * @param Uobject - 包含目标方法的游戏对象实例
 * @note 使用类型断言(as any)绕过TS类型检查，运行时动态调用对象的指定方法
 * @behavior 当FCall事件触发时，在目标对象上执行指定名称的方法 */
GameInstance.FCall.Bind((FunctionName, Uobject) => {
    (Uobject as any)[FunctionName]();
});

// 导入Mixin模块
import './Project/Blueprints/Character/BP_MainCharacter';
import './Project/Blueprints/Input/AC_Input';
