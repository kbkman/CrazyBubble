#pragma once

#define CBCPHEAD            0x4342                        //������Ѷͨ��Ϣͷ������CB��

#define CPORDER_LOGIN                           1         //�����û������½������/�ͣ���½�������ɹ�
#define CPORDER_ERROR                           2        //�ͣ���������
	#define CPORDER_ERROR_TYPE_GAMESTART          1          //������Ϸ�Ѿ���ʼ
	#define CPORDER_ERROR_TYPE_HASINROOM         2          //�����Ѿ������˸÷���
	#define CPORDER_ERROR_TYPE_ROOMMAXUSER    3          //�����������
#define CPORDER_ROOMLIST                    16        //�����û����󷿼��б�
#define CPORDER_ENTERROOM                 3          //�����û�������뷿��/�ͣ��û��Ѿ������˷���
#define CPORDER_QUITROOM                   4          //�����û������뿪����/�ͣ��û��Ѿ��뿪�˷���
#define CPORDER_RLBEGIN                       5         //�ͣ���ʼˢ�·����б�
#define CPORDER_ROOMINFO                    7        //�ͣ�������Ϣ
#define CPORDER_RLEND                           6         //�ͣ�����ˢ�·����б�
#define CPORDER_PREPAREGAME              9         //�����û�׼���ÿ�ʼ��Ϸ��/�ͣ����û�׼����ʼ��Ϸ��
#define CPORDER_STARTGAME                  10       //�ͣ���Ϸ��ʼ��
#define CPORDER_STOPGAME                   11       //�ͣ���Ϸֹͣ��
#define CPORDER_GAME                           12       //�����û�����Ϸ��Ϣ/�ͣ��û�����Ϸ��Ϣ
#define CPORDER_GAME_SHOOTBUBBLE                                1      //���ͣ��û���Ϸ��Ϣ����������
#define CPORDER_GAME_SHOOTBADBUBBLE                          2     //���ͣ��û���Ϸ��Ϣ�����仵����
#define CPORDER_GAME_ERASEBUBBLE                                 3     //���ͣ��û���Ϸ��Ϣ����������
#define CPORDER_GAME_ADDLINEBADBUBBLE                       4     //���ͣ��û���Ϸ��Ϣ��������л�����
#define CPORDER_GAME_UNKNOWBUBBLECOLOR                  5    //���ͣ��û���Ϸ��Ϣ������������ɫ
#define CPORDER_GAME_THROWSHIT                                    6     //���ͣ��û���Ϸ��Ϣ�����ӱ��
#define CPORDER_GAME_ADJUSTTOOLORDER                        15   //���ͣ��û���Ϸ��Ϣ���л�����˳��
#define CPORDER_GAME_TOOL_THROWBADBUBBLE               7    //���ͣ��û���Ϸ������Ϣ���û�����һ���û�ʹ�õ��� �ӻ�����
#define CPORDER_GAME_TOOL_ERASEBUBBLE                       8    //���ͣ��û���Ϸ������Ϣ���û�����һ���û�ʹ�õ��� ��������
#define CPORDER_GAME_TOOL_THROWLINEBADBUBBLE        9    //���ͣ��û���Ϸ������Ϣ���û�����һ���û�ʹ�õ��� �������л�����
#define CPORDER_GAME_TOOL_CHANGETOOLMODE              10    //���ͣ��û���Ϸ������Ϣ���û��л�����ģʽ
#define CPORDER_GAME_TOOL_REMOVETOOL                       11    //���ͣ��û���Ϸ������Ϣ���û��Ƴ����ߣ����û�ʹ���˵��ߣ�������Ҫ���٣�
#define CPORDER_GAME_TOOL_UNKNOWBUBBLECOLOR       12  //���ͣ��û���Ϸ������Ϣ���û�����һ���û�ʹ�õ��� ����������ɫ
#define CPORDER_GAME_TOOL_THROWSHIT                         13  //���ͣ��û���Ϸ������Ϣ���û�����һ���û�ʹ�õ��� �ӱ��
#define CPORDER_GAME_OVER                                              14  //�����û���Ϸ��Ϣ����Ϸʧ��

#define CPORDER_USERENTERRROOM     13       //�ͣ������û����뷿��
#define CPORDER_USERLEAVEROOM       14       //�ͣ����û��뿪����
#define CPORDER_ROOMUSER                 15       //�ͣ������û���Ϣ