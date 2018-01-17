#include <stdio.h>
#include <unistd.h>
#include "service.h"
#include "modbusconfig.h"
#include "modbusmanager.h"


void MakeValueFile(list<Value>& v)
{
	FILE *fp = fopen("value.html", "w");
	if( NULL == fp )
	{
		return;
	}
	for(list<Value>::iterator i = v.begin(); i != v.end(); i++)
	{
		char buf[1024];
		snprintf(buf, sizeof(buf), "<p>[%s][%s][%08X][%04X][%-4d][t=%d]</p>", 
				i->GetName().data(),
				i->GetCom().data(), 
				i->GetId(), 
				i->GetValue(),
				i->GetValue(),
				i->mdiff());
		fwrite(buf, string(buf).length(), 1, fp);
	}
	fclose(fp);
}
void ShowValue(const Value& value)
{
	printf("[%s][%s][%08X][%04X][%-4d][t=%d]\n", 
			value.GetName().data(),
			value.GetCom().data(), 
			value.GetId(), 
			value.GetValue(),
			value.GetValue(),
			value.mdiff());
}

int main(void)
{
	Service service;
	TimeOperator timer;
	TimeOperator ftimer;
	ModbusManager manager;
	string server_path = ".modbus.service";

#if 0
	manager.SetComConfig( ComConfig("/dev/ttySX0", 9600, 0, 8, 1) );
	manager.SetVarConfig( VarConfig("/dev/ttySX0", VarCmdAdd, 1, 3, 0, 9, 80) );
	manager.SetVarName( VarName("/dev/ttySX0", "1IN1", 0x01, 0x03, 0x0000) );
	manager.SetVarName( VarName("/dev/ttySX0", "1IN2", 0x01, 0x03, 0x0001) );
	manager.SetVarName( VarName("/dev/ttySX0", "1IN3", 0x01, 0x03, 0x0002) );
	manager.SetVarName( VarName("/dev/ttySX0", "1IN4", 0x01, 0x03, 0x0003) );
	manager.SetVarName( VarName("/dev/ttySX0", "1IN5", 0x01, 0x03, 0x0004) );
	manager.SetVarName( VarName("/dev/ttySX0", "1IN6", 0x01, 0x03, 0x0005) );
	manager.SetVarName( VarName("/dev/ttySX0", "1IN7", 0x01, 0x03, 0x0006) );
	manager.SetVarName( VarName("/dev/ttySX0", "1IN8", 0x01, 0x03, 0x0007) );
	manager.SetVarName( VarName("/dev/ttySX0", "1IN9", 0x01, 0x03, 0x0008) );
#endif
	if( service.StartServer(server_path) == false )
	{
		printf("StartServer(%s) failed\n", server_path.data());
		return -1;
	}
                                                                              
	while(1)
	{
		if( service.RecvPacket() )
		{
			ModbusConfig &p = *(ModbusConfig*)service.GetData();
			switch(p.GetPacketType())
			{
				case TypeVarName:
					manager.SetVarName( p.GetVarName() );
					break;
				case TypeVarConfig:
					manager.SetVarConfig( p.GetVarConfig() );
					break;
				case TypeComConfig:
					manager.SetComConfig( p.GetComConfig() );
					break;
			}
		}
		manager.RunLoop();
		if( timer.mdiff() > 300 )
		{
			system("clear");
			manager.GetValue(ShowValue);
			timer.init();
		}
		if( ftimer.sdiff() > 0 )
		{
			list<Value> v;
			manager.GetValue(v);
			MakeValueFile(v);
			ftimer.init();
		}
		usleep(100);
	}

	return 0;
}