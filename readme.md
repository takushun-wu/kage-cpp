# kage-cpp
可自动生成汉字的[Kage（“影”）引擎](https://github.com/kurgm/kage-engine)的一种C++实现，基于ge9的[改版Kage引擎](https://github.com/ge9/kage-engine-2)。本项目遵循[GPLv3协议](https://www.gnu.org/licenses/gpl-3.0.en.html)。
## Kage引擎简介
Kage引擎是上地宏一（Kamichi Koichi）开发的一种基于控制点以及部件引用的汉字字形生成系统，[GlyphWiki](https://zhs.glyphwiki.org/)是一种基于Kage引擎的汉字字形数据库平台。其主要特点是只需对汉字字形（及部件）进行描述就可生成大量的汉字字形。部件引用是Kage引擎的一大重要功能，这大大增加了字形制作的效率，但这也带来了一些局限性。总体上来说为大量生僻汉字的制造带来了可能性，具有重要的实用和研究意义。
## 与上述Kage引擎的不同之处与优化
### 原版Kage引擎
- 曲线笔画（如撇、点、捺）采用[曲线拟合算法](https://github.com/erich666/GraphicsGems/blob/master/gems/FitCurves.c)，生成贝塞尔三次曲线，而非分段折线；
- 对竖、撇、提的衬线以及笔画尾部的钩挑造型进行优化。
### ge9的改版Kage引擎
- 修复在某些情况（调整参数为负数时）下笔画顶部/尾部部分缺失的问题（参考[HowardZorn的Python实现版本](https://github.com/HowardZorn/kage-engine)）；
- 修复生成黑体时字形无法90°/180°/270°/翻转的问题；
- 修改了曲线拟合算法的最大容错值；
- 修改了在部分非法条件的行为，使之与原版Kage引擎一致。
### 以上两种引擎的共性问题的优化
- 修复生成黑体时“折弯”笔画缺失的问题；
- 优化数据结构，使字形生成逻辑更加清晰；
- 使用C++编写，生成效率显著提高。
## 使用样例
本项目的C++代码须使用支持C++11的编译器进行编译。
```c++
#include <iostream>
#include <unordered_map>
#include <vector>

#include "export.h"
#include "gwdata.h"
#include "kage.h"

int main(int argc, char** argv) {
    // 首先建立Kage对象，字体和粗细可自定义，默认为宋体
    Kage::Kage kage;

    // 向Kage对象存入部件/字形（此处仅为范例）
    const std::vector<std::string> glyphsList={"test", "u6f22", "u6c35-07", "u26c29-07"};
    kage.PushBuhin("test", "1:12:13:25:28:24:95$1:2:2:25:28:81:28$1:22:23:81:28:80:95$1:32:32:53:28:52:95$1:0:0:12:61:100:61$1:2:2:24:95:80:95$1:0:4:108:21:110:93$1:0:0:165:23:165:46$1:0:413:142:15:132:63$1:2:0:132:63:175:57$1:0:313:139:79:139:99$1:2:2:139:99:160:99$1:0:24:160:66:160:99$2:0:7:30:130:27:167:11:187$2:0:7:27:184:47:178:65:165$2:0:5:37:110:44:133:57:152$2:7:4:119:104:135:117:135:134$2:32:7:84:106:77:124:62:142$1:0:2:59:106:102:106$3:22:5:102:106:96:165:122:165$3:0:0:78:140:76:180:117:180$4:0:5:164:109:131:179:177:179$6:7:8:132:106:155:112:146:145:171:139$7:0:7:184:14:184:88:184:146:166:171");
    kage.PushBuhin("u6f22", "99:150:0:9:12:73:200:u6c35-07:0:-10:50$99:0:0:54:10:190:199:u26c29-07");
    kage.PushBuhin("u6c35-07", "2:7:8:33:20:59:28:69:41$2:7:8:12:68:38:75:49:89$2:7:8:14:133:54:142:50:184$2:12:7:47:143:49:138:86:58");
    kage.PushBuhin("u26c29-07", "1:0:0:18:29:187:29$1:0:0:73:10:73:48$1:0:0:132:10:132:48$1:12:13:44:59:44:87$1:2:2:44:59:163:59$1:22:23:163:59:163:87$1:2:2:44:87:163:87$1:0:0:32:116:176:116$1:0:0:21:137:190:137$7:32:7:102:59:102:123:102:176:10:190$2:7:0:105:137:126:169:181:182");

    // 也可选择在生成字形时从数据库读取字形
    // 函数类型必须为std::function<std::string(std::string)>
    // 输入参数为字形名称，输出GlyphWiki格式数据。若找不到字形，请返回空字符串
    // kage.SetDBsearchCallback(FetchGWdataFromDB);

    // 缺失部件时的替代字形也可以自定义（仅生成部分）
    kage.SetNotDefGlyph(Kage::StrokesParse("2:32:0:19:13:19:100:19:187$2:32:0:181:13:181:100:181:187$2:32:0:13:19:100:19:187:19$2:32:0:13:181:100:181:187:181$2:32:32:20:20:100:100:180:180$2:32:32:180:20:100:100:20:180"));

    // 生成汉字字形
    std::unordered_map<std::string, Kage::Canva> glyphsCanvas;
    for(auto i: glyphsList) {
        // 首先检查字形是否存在部件缺失和循环引用问题
        int checkRes = kage.CheckGlyph(i);
        if(checkRes != Kage::CHECK_OK) {
            std::cerr << "Unable to generate glyph '" << i << "', skipped." << std::endl;
            continue;
        }
        // 定义画布，并生成字形
        // Kage::MakeGlyph函数不执行部件检查
        Kage::Canva ca;
        kage.MakeGlyph(ca, i);
        glyphsCanvas.insert({i, ca});
    }

    // 将字形导出为SVG（或Fontforge SFD格式）
    for(auto i: glyphsCanvas) {
        std::cout << "==== " << i.first << " ====" << std::endl;
        std::cout << Kage::Canva2SVG(i.second) << std::endl;
    }

    return 0;
}
```
## 关于授权协议
- 该项目的上游（原版以及ge9版Kage引擎）采用GPLv3协议，根据协议的要求，本项目也同样采用GPLv3协议；
- 本存储库允许免费使用、复制、修改、分发，不论是否构成商业性使用；
- 对外分发**必须提供项目的源代码，且只可以GPLv3协议发布**，即GPLv3的“传染性”。若您打算基于本项目开发并对外提供闭源软件**请慎重考虑**；
- 基于GlyphWiki数据生成的汉字不受GPLv3协议的约束，[详见此处](https://zhs.glyphwiki.org/wiki/GlyphWiki:%e8%91%97%e4%bd%9c%e6%9d%83%e4%b8%8e%e8%ae%b8%e5%8f%af%e5%8d%8f%e8%ae%ae)。
- 关于GPLv3协议的更多内容，请[点击此处](https://www.gnu.org/licenses/gpl-3.0.en.html)。
## 使用的第三方库列表
- [utfcpp](https://github.com/nemtrif/utfcpp)

