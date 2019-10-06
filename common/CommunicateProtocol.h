#pragma once

#define CBCPHEAD            0x4342                        //泡泡龙讯通信息头（即：CB）

#define CPORDER_LOGIN                           1         //服－用户请求登陆服务器/客－登陆服务器成功
#define CPORDER_ERROR                           2        //客－发生错误
	#define CPORDER_ERROR_TYPE_GAMESTART          1          //服－游戏已经开始
	#define CPORDER_ERROR_TYPE_HASINROOM         2          //服－已经进入了该房间
	#define CPORDER_ERROR_TYPE_ROOMMAXUSER    3          //服－房间客满
#define CPORDER_ROOMLIST                    16        //服－用户请求房间列表
#define CPORDER_ENTERROOM                 3          //服－用户请求进入房间/客－用户已经进入了房间
#define CPORDER_QUITROOM                   4          //服－用户请求离开房间/客－用户已经离开了房间
#define CPORDER_RLBEGIN                       5         //客－开始刷新房间列表
#define CPORDER_ROOMINFO                    7        //客－房间信息
#define CPORDER_RLEND                           6         //客－结束刷新房间列表
#define CPORDER_PREPAREGAME              9         //服－用户准备好开始游戏了/客－有用户准备开始游戏了
#define CPORDER_STARTGAME                  10       //客－游戏开始了
#define CPORDER_STOPGAME                   11       //客－游戏停止了
#define CPORDER_GAME                           12       //服－用户的游戏信息/客－用户的游戏信息
#define CPORDER_GAME_SHOOTBUBBLE                                1      //服客－用户游戏信息：发射泡泡
#define CPORDER_GAME_SHOOTBADBUBBLE                          2     //服客－用户游戏信息：发射坏泡泡
#define CPORDER_GAME_ERASEBUBBLE                                 3     //服客－用户游戏信息：消除泡泡
#define CPORDER_GAME_ADDLINEBADBUBBLE                       4     //服客－用户游戏信息：添加两行坏泡泡
#define CPORDER_GAME_UNKNOWBUBBLECOLOR                  5    //服客－用户游戏信息：隐藏泡泡颜色
#define CPORDER_GAME_THROWSHIT                                    6     //服客－用户游戏信息：受扔便便
#define CPORDER_GAME_ADJUSTTOOLORDER                        15   //服客－用户游戏信息：切换道具顺序
#define CPORDER_GAME_TOOL_THROWBADBUBBLE               7    //服客－用户游戏道具信息：用户向另一个用户使用道具 扔坏泡泡
#define CPORDER_GAME_TOOL_ERASEBUBBLE                       8    //服客－用户游戏道具信息：用户向另一个用户使用道具 擦除泡泡
#define CPORDER_GAME_TOOL_THROWLINEBADBUBBLE        9    //服客－用户游戏道具信息：用户向另一个用户使用道具 增加两行坏泡泡
#define CPORDER_GAME_TOOL_CHANGETOOLMODE              10    //服客－用户游戏道具信息：用户切换道具模式
#define CPORDER_GAME_TOOL_REMOVETOOL                       11    //服客－用户游戏道具信息：用户移除道具（即用户使用了道具，道具需要减少）
#define CPORDER_GAME_TOOL_UNKNOWBUBBLECOLOR       12  //服客－用户游戏道具信息：用户向另一个用户使用道具 隐藏泡泡颜色
#define CPORDER_GAME_TOOL_THROWSHIT                         13  //服客－用户游戏道具信息：用户向另一个用户使用道具 扔便便
#define CPORDER_GAME_OVER                                              14  //服－用户游戏信息：游戏失败

#define CPORDER_USERENTERRROOM     13       //客－有新用户进入房间
#define CPORDER_USERLEAVEROOM       14       //客－有用户离开房间
#define CPORDER_ROOMUSER                 15       //客－房间用户信息