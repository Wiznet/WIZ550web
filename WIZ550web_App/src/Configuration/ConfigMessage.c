#include <string.h>

#include "stm32f10x.h"
#include "common.h"
#include "socket.h"
#include "ConfigData.h"
#include "ConfigMessage.h"
#include "storageHandler.h"

typedef struct __Remote_Info {
	uint8_t ip[4];
	uint16_t port;
} __attribute__((packed)) Remote_Info;

typedef struct __Firmware_Upload_Info {
	WIZnet_Header wiznet_header;
	Remote_Info configtool_info;
	Remote_Info tftp_info;
	uint8_t filename[50];
} __attribute__((packed)) Firmware_Upload_Info;

static void encrypt(uint8_t key, uint8_t *buffer, uint16_t len)
{
	uint16_t i;
	for(i=0; i<len; i++) {
		buffer[i] ^= key;
	}
}

static void decrypt(uint8_t key, uint8_t *buffer, uint16_t len)
{
	uint16_t i;
	for(i=0; i<len; i++) {
		buffer[i] ^= key;
	}
}

static void process_discovery(uint8_t sock, Remote_Info *remote_info, WIZnet_Header *wiznet_header)
{
	uint16_t len;
	uint8_t buffer[sizeof(WIZnet_Header) + sizeof(WIZnet_Discovery_Mixed_Condition)];	// header + most biggest structure
	uint32_t ptr;
	WIZnet_Discovery_Reply reply;
	S2E_Packet *s2e_packet = get_S2E_Packet_pointer();
	uint8_t ip[4];
	uint16_t port;

	getsockopt(sock, SO_REMAINSIZE, &len);
	if(len != wiznet_header->length)
		return;

	switch(wiznet_header->op_code[0]) {
	case DISCOVERY_ALL:
		break;

	case DISCOVERY_PRODUCT_CODE:
	{
		WIZnet_Discovery_Product_Code product_code;
		recvfrom(sock, (uint8_t *)&product_code, sizeof(WIZnet_Discovery_Product_Code), ip, &port);

		if((wiznet_header->valid & 0x80))
			decrypt((wiznet_header->valid & 0x7F), (uint8_t *)&product_code, len);

		if(memcmp(product_code.product_code, s2e_packet->module_type, 3))
			return;
		break;
	}

	case DISCOVERY_MAC_ADDRESS:
	{
		WIZnet_Discovery_MAC_Address mac_address;
		recvfrom(sock, (uint8_t *)&mac_address, sizeof(WIZnet_Discovery_MAC_Address), ip, &port);

		if((wiznet_header->valid & 0x80))
			decrypt((wiznet_header->valid & 0x7F), (uint8_t *)&mac_address, len);

		if(   mac_address.start_mac_address[0] > s2e_packet->network_info_common.mac[0]
		   || mac_address.start_mac_address[1] > s2e_packet->network_info_common.mac[1]
		   || mac_address.start_mac_address[2] > s2e_packet->network_info_common.mac[2]
		   || mac_address.start_mac_address[3] > s2e_packet->network_info_common.mac[3]
		   || mac_address.start_mac_address[4] > s2e_packet->network_info_common.mac[4]
		   || mac_address.start_mac_address[5] > s2e_packet->network_info_common.mac[5])
			return;
		if(   mac_address.end_mac_address[0] < s2e_packet->network_info_common.mac[0]
		   || mac_address.end_mac_address[1] < s2e_packet->network_info_common.mac[1]
		   || mac_address.end_mac_address[2] < s2e_packet->network_info_common.mac[2]
		   || mac_address.end_mac_address[3] < s2e_packet->network_info_common.mac[3]
		   || mac_address.end_mac_address[4] < s2e_packet->network_info_common.mac[4]
		   || mac_address.end_mac_address[5] < s2e_packet->network_info_common.mac[5])
			return;
		break;
	}

	case DISCOVERY_ALIAS:
	{
		WIZnet_Discovery_Alias alias;
		recvfrom(sock, (uint8_t *)&alias, sizeof(WIZnet_Discovery_Alias), ip, &port);

		if((wiznet_header->valid & 0x80))
			decrypt((wiznet_header->valid & 0x7F), (uint8_t *)&alias, len);

		if(memcmp(alias.alias, s2e_packet->module_name, strlen((char *)alias.alias)))
			return;
		break;
	}

	case DISCOVERY_MIXED_COND:
	{
		WIZnet_Discovery_Mixed_Condition mixed_condition;
		recvfrom(sock, (uint8_t *)&mixed_condition, sizeof(WIZnet_Discovery_Mixed_Condition), ip, &port);

		if((wiznet_header->valid & 0x80))
			decrypt((wiznet_header->valid & 0x7F), (uint8_t *)&mixed_condition, len);

		break;
	}

	default:
		return;
	}

	// reply
	wiznet_header->length = sizeof(WIZnet_Discovery_Reply);
	wiznet_header->op_code[1] = WIZNET_REPLY;

	memcpy(reply.product_code, s2e_packet->module_type, 3);
	memcpy(reply.fw_version, s2e_packet->fw_ver, 3);
	memcpy(reply.mac_address, s2e_packet->network_info_common.mac, 6);

	if((wiznet_header->valid & 0x80))
		encrypt((wiznet_header->valid & 0x7F), (uint8_t *)&reply, sizeof(WIZnet_Discovery_Reply));

	ptr = 0;
	memcpy(buffer, wiznet_header, sizeof(WIZnet_Header));
	ptr += sizeof(WIZnet_Header);
	memcpy(buffer + ptr, (void *)&reply, sizeof(WIZnet_Discovery_Reply));
	ptr += sizeof(WIZnet_Discovery_Reply);

	if(wiznet_header->unicast == 0) {
		memset(remote_info->ip, 255, 4);
	}
	sendto(sock, buffer, ptr, remote_info->ip, remote_info->port);
}

static void process_get_info(uint8_t sock, Remote_Info *remote_info, WIZnet_Header *wiznet_header)
{
	uint16_t len;
	uint8_t buffer[sizeof(WIZnet_Header) + sizeof(WIZnet_Get_Info_Reply)];
	uint32_t ptr;
	WIZnet_Get_Info get_info;
	WIZnet_Get_Info_Reply reply;
	S2E_Packet *s2e_packet = get_S2E_Packet_pointer();
	uint8_t ip[4];
	uint16_t port;

	getsockopt(sock, SO_REMAINSIZE, &len);
	if(len != wiznet_header->length)
		return;

	recvfrom(sock, (uint8_t *)&get_info, sizeof(WIZnet_Get_Info), ip, &port);

	if((wiznet_header->valid & 0x80))
		decrypt((wiznet_header->valid & 0x7F), (uint8_t *)&get_info, len);

	if(memcmp(get_info.dst_mac_address, s2e_packet->network_info_common.mac, 6))
		return;

	// reply
	wiznet_header->length = sizeof(WIZnet_Get_Info_Reply);
	wiznet_header->op_code[1] = WIZNET_REPLY;

	memcpy(reply.src_mac_address, s2e_packet->network_info_common.mac, 6);
	memcpy(&reply.system_info, s2e_packet, sizeof(S2E_Packet));

	if((wiznet_header->valid & 0x80))
		encrypt((wiznet_header->valid & 0x7F), (uint8_t *)&reply, sizeof(WIZnet_Get_Info_Reply));

	ptr = 0;
	memcpy(buffer, wiznet_header, sizeof(WIZnet_Header));
	ptr += sizeof(WIZnet_Header);
	memcpy(buffer + ptr, (void *)&reply, sizeof(WIZnet_Get_Info_Reply));
	ptr += sizeof(WIZnet_Get_Info_Reply);

	if(wiznet_header->unicast == 0) {
		memset(remote_info->ip, 255, 4);
	}
	sendto(sock, buffer, ptr, remote_info->ip, remote_info->port);
}

static void process_set_info(uint8_t sock, Remote_Info *remote_info, WIZnet_Header *wiznet_header)
{
	uint16_t len;
	uint8_t buffer[sizeof(WIZnet_Header) + sizeof(WIZnet_Set_Info_Reply)];
	uint32_t ptr;
	WIZnet_Set_Info set_info;
	WIZnet_Set_Info_Reply reply;
	S2E_Packet *s2e_packet = get_S2E_Packet_pointer();
	uint8_t ip[4];
	uint16_t port;

	getsockopt(sock, SO_REMAINSIZE, &len);
	if(len != wiznet_header->length)
		return;

	recvfrom(sock, (uint8_t *)&set_info, sizeof(WIZnet_Set_Info), ip, &port);

	if((wiznet_header->valid & 0x80))
		decrypt((wiznet_header->valid & 0x7F), (uint8_t *)&set_info, len);

	if(memcmp(set_info.dst_mac_address, s2e_packet->network_info_common.mac, 6))
		return;

	if(strcmp((char *)set_info.set_pw, (char *)s2e_packet->options.pw_setting))
		return;

	//update config msg
	set_S2E_Packet_value(s2e_packet, &set_info.system_info, sizeof(S2E_Packet));
	s2e_packet->fw_ver[0] = MAJOR_VER;
	s2e_packet->fw_ver[1] = MINOR_VER;
	s2e_packet->fw_ver[2] = MAINTENANCE_VER;

	//save to storage
	save_S2E_Packet_to_storage();

	// reply
	wiznet_header->length = sizeof(WIZnet_Set_Info_Reply);
	wiznet_header->op_code[1] = WIZNET_REPLY;

	memcpy(reply.src_mac_address, s2e_packet->network_info_common.mac, 6);
	memcpy(&reply.system_info, s2e_packet, sizeof(S2E_Packet));

	if((wiznet_header->valid & 0x80))
		encrypt((wiznet_header->valid & 0x7F), (uint8_t *)&reply, sizeof(WIZnet_Set_Info_Reply));

	ptr = 0;
	memcpy(buffer, wiznet_header, sizeof(WIZnet_Header));
	ptr += sizeof(WIZnet_Header);
	memcpy(buffer + ptr, (void *)&reply, sizeof(WIZnet_Set_Info_Reply));
	ptr += sizeof(WIZnet_Set_Info_Reply);

	if(wiznet_header->unicast == 0) {
		memset(remote_info->ip, 255, 4);
	}
	sendto(sock, buffer, ptr, remote_info->ip, remote_info->port);

	NVIC_SystemReset();
}

static void process_firmware_upload_init(uint8_t sock, Remote_Info *remote_info, WIZnet_Header *wiznet_header)
{
	S2E_Packet *value = get_S2E_Packet_pointer();
	struct __network_info_common tmp;
	uint16_t len;
	WIZnet_Firmware_Upload_Init firmware_upload;
	uint8_t buffer[sizeof(WIZnet_Header) + sizeof(WIZnet_Firmware_Upload_Done_Reply)];
	uint32_t ptr;
	WIZnet_Firmware_Upload_Init_Reply reply;
	Firmware_Upload_Info firmware_upload_info;
	S2E_Packet *s2e_packet = get_S2E_Packet_pointer();
	uint8_t ip[4];
	uint16_t port;

	getsockopt(sock, SO_REMAINSIZE, &len);
	if(len != wiznet_header->length)
		return;

	recvfrom(sock, (uint8_t *)&firmware_upload, sizeof(WIZnet_Firmware_Upload_Init), ip, &port);

	if(wiznet_header->op_code[0] != FIRMWARE_UPLOAD_INIT)
		return;

	if((wiznet_header->valid & 0x80))
		decrypt((wiznet_header->valid & 0x7F), (uint8_t *)&firmware_upload, len);

	if(memcmp(firmware_upload.dst_mac_address, s2e_packet->network_info_common.mac, 6))
		return;
	
	if(strcmp((char *)firmware_upload.set_pw, (char *)s2e_packet->options.pw_setting))
		return;

	/* Storee Current Network Information (For DHCP) */
	memcpy(&tmp, &(value->network_info_common), sizeof(struct __network_info_common));
	load_S2E_Packet_from_storage();
	memcpy(&(value->network_info_common), &tmp, sizeof(struct __network_info_common));
	save_S2E_Packet_to_storage();

	//write the TFTP server ip, port, filename and etc. to storage
	memcpy(&firmware_upload_info.wiznet_header                , wiznet_header                            , sizeof(WIZnet_Header));
	memcpy(&firmware_upload_info.configtool_info              , remote_info                              , sizeof(Remote_Info));
	memcpy(firmware_upload_info.tftp_info.ip                  , (uint8_t *) (firmware_upload.server_ip)  , sizeof(uint8_t)*4);
	memcpy((uint8_t *)&(firmware_upload_info.tftp_info.port)  , (uint8_t *)&(firmware_upload.server_port), sizeof(uint8_t)*2);
	memcpy(firmware_upload_info.filename                      , (uint8_t *) (firmware_upload.file_name)  , sizeof(uint8_t)*50);
	write_storage(0, &firmware_upload_info, sizeof(Firmware_Upload_Info));

	// reply
	wiznet_header->length = sizeof(WIZnet_Firmware_Upload_Init_Reply);
	wiznet_header->op_code[1] = WIZNET_REPLY;

	memcpy(reply.src_mac_address, s2e_packet->network_info_common.mac, 6);

	if((wiznet_header->valid & 0x80))
		encrypt((wiznet_header->valid & 0x7F), (uint8_t *)&reply, sizeof(WIZnet_Firmware_Upload_Init_Reply));

	ptr = 0;
	memcpy(buffer, wiznet_header, sizeof(WIZnet_Header));
	ptr += sizeof(WIZnet_Header);
	memcpy(buffer + ptr, (void *)&reply, sizeof(WIZnet_Firmware_Upload_Init_Reply));
	ptr += sizeof(WIZnet_Firmware_Upload_Init_Reply);

	if(wiznet_header->unicast == 0) {
		memset(remote_info->ip, 255, 4);
	}
	sendto(sock, buffer, ptr, remote_info->ip, remote_info->port);

	// Reboot to bootloader
	NVIC_SystemReset();
}

void reply_firmware_upload_done(uint8_t sock)
{
	uint8_t buffer[sizeof(WIZnet_Header) + sizeof(WIZnet_Firmware_Upload_Done_Reply)];
	uint32_t ptr;
	WIZnet_Firmware_Upload_Done_Reply reply;
	Firmware_Upload_Info firmware_upload_info;
	S2E_Packet *s2e_packet = get_S2E_Packet_pointer();
	uint8_t ip[4];
	uint16_t port;

	close(sock);
	socket(sock, Sn_MR_UDP, REMOTE_CLIENT_PORT, 0);

	read_storage(0, &firmware_upload_info, sizeof(Firmware_Upload_Info));

	// reply
	firmware_upload_info.wiznet_header.length = sizeof(WIZnet_Firmware_Upload_Done_Reply);
	firmware_upload_info.wiznet_header.op_code[0] = FIRMWARE_UPLOAD_DONE;
	firmware_upload_info.wiznet_header.op_code[1] = WIZNET_REPLY;

	memcpy(reply.src_mac_address, s2e_packet->network_info_common.mac, 6);

	if((firmware_upload_info.wiznet_header.valid & 0x80))
		encrypt((firmware_upload_info.wiznet_header.valid & 0x7F), (uint8_t *)&reply, sizeof(WIZnet_Firmware_Upload_Done_Reply));

	ptr = 0;
	memcpy(buffer, &firmware_upload_info.wiznet_header, sizeof(WIZnet_Header));
	ptr += sizeof(WIZnet_Header);
	memcpy(buffer + ptr, (void *)&reply, sizeof(WIZnet_Firmware_Upload_Done_Reply));
	ptr += sizeof(WIZnet_Firmware_Upload_Done_Reply);

	if(firmware_upload_info.wiznet_header.unicast == 0) {
		memset(firmware_upload_info.configtool_info.ip, 255, 4);
	}
	memcpy(ip, firmware_upload_info.configtool_info.ip, sizeof(uint8_t) * 4);
	memcpy(&port, &firmware_upload_info.configtool_info.port, sizeof(uint16_t));
	sendto(sock, buffer, ptr, ip, port);
}

static void process_remote_reset(uint8_t sock, Remote_Info *remote_info, WIZnet_Header *wiznet_header)
{
	uint16_t len;
	uint8_t buffer[sizeof(WIZnet_Header) + sizeof(WIZnet_Reset_Reply)];
	uint32_t ptr;
	WIZnet_Reset reset;
	WIZnet_Reset_Reply reply;
	S2E_Packet *s2e_packet = get_S2E_Packet_pointer();
	uint8_t ip[4];
	uint16_t port;

	getsockopt(sock, SO_REMAINSIZE, &len);
	if(len != wiznet_header->length)
		return;

	recvfrom(sock, (uint8_t *)&reset, sizeof(WIZnet_Reset), ip, &port);

	if((wiznet_header->valid & 0x80))
		decrypt((wiznet_header->valid & 0x7F), (uint8_t *)&reset, len);

	if(memcmp(reset.dst_mac_address, s2e_packet->network_info_common.mac, 6))
		return;

	if(strcmp((char *)reset.set_pw, (char *)s2e_packet->options.pw_setting))
		return;

	// reply
	wiznet_header->length = sizeof(WIZnet_Reset_Reply);
	wiznet_header->op_code[1] = WIZNET_REPLY;

	memcpy(reply.src_mac_address, s2e_packet->network_info_common.mac, 6);

	if((wiznet_header->valid & 0x80))
		encrypt((wiznet_header->valid & 0x7F), (uint8_t *)&reply, sizeof(WIZnet_Reset_Reply));

	ptr = 0;
	memcpy(buffer, wiznet_header, sizeof(WIZnet_Header));
	ptr += sizeof(WIZnet_Header);
	memcpy(buffer + ptr, (void *)&reply, sizeof(WIZnet_Reset_Reply));
	ptr += sizeof(WIZnet_Reset_Reply);

	if(wiznet_header->unicast == 0) {
		memset(remote_info->ip, 255, 4);
	}
	sendto(sock, buffer, ptr, remote_info->ip, remote_info->port);

	// Reboot to bootloader
	NVIC_SystemReset();
}

static void process_factory_reset(uint8_t sock, Remote_Info *remote_info, WIZnet_Header *wiznet_header)
{
	uint16_t len;
	uint8_t buffer[sizeof(WIZnet_Header) + sizeof(WIZnet_Factory_Reset_Reply)];
	uint32_t ptr;
	WIZnet_Factory_Reset factory_reset;
	WIZnet_Factory_Reset_Reply reply;
	S2E_Packet *s2e_packet = get_S2E_Packet_pointer();
	uint8_t ip[4];
	uint16_t port;

	getsockopt(sock, SO_REMAINSIZE, &len);
	if(len != wiznet_header->length)
		return;

	recvfrom(sock, (uint8_t *)&factory_reset, sizeof(WIZnet_Factory_Reset), ip, &port);

	if((wiznet_header->valid & 0x80))
		decrypt((wiznet_header->valid & 0x7F), (uint8_t *)&factory_reset, len);

	if(memcmp(factory_reset.dst_mac_address, s2e_packet->network_info_common.mac, 6))
		return;

	if(strcmp((char *)factory_reset.set_pw, (char *)s2e_packet->options.pw_setting))
		return;

	// set factory value
	set_S2E_Packet_to_factory_value();
	save_S2E_Packet_to_storage();

	// reply
	wiznet_header->length = sizeof(WIZnet_Reset_Reply);
	wiznet_header->op_code[1] = WIZNET_REPLY;

	memcpy(reply.src_mac_address, s2e_packet->network_info_common.mac, 6);

	if((wiznet_header->valid & 0x80))
		encrypt((wiznet_header->valid & 0x7F), (uint8_t *)&reply, sizeof(WIZnet_Factory_Reset_Reply));

	ptr = 0;
	memcpy(buffer, wiznet_header, sizeof(WIZnet_Header));
	ptr += sizeof(WIZnet_Header);
	memcpy(buffer + ptr, (void *)&reply, sizeof(WIZnet_Factory_Reset_Reply));
	ptr += sizeof(WIZnet_Factory_Reset_Reply);

	if(wiznet_header->unicast == 0) {
		memset(remote_info->ip, 255, 4);
	}
	sendto(sock, buffer, ptr, remote_info->ip, remote_info->port);

	// Reboot to bootloader
	NVIC_SystemReset();
}

void do_udp_config(uint8_t sock)
{
	uint8_t sckState;
	uint16_t len;
	Remote_Info remote_info;
	WIZnet_Header wiznet_header;

	getsockopt(sock, SO_STATUS, &sckState);
	switch (sckState) {
	case SOCK_UDP:
		getsockopt(sock, SO_RECVBUF, &len);
		if (len >= sizeof(WIZnet_Header)) {
			recvfrom(sock, (uint8_t *)&wiznet_header, sizeof(WIZnet_Header), remote_info.ip, &remote_info.port);

			if(wiznet_header.stx != STX)
				break;
			if(wiznet_header.op_code[1] != WIZNET_REQUEST)
				break;

			switch(wiznet_header.op_code[0] & 0xF0) {
			case DISCOVERY:
				process_discovery(sock, &remote_info, &wiznet_header);
				break;
			case GET_INFO:
				process_get_info(sock, &remote_info, &wiznet_header);
				break;
			case SET_INFO:
				process_set_info(sock, &remote_info, &wiznet_header);
				break;
			case FIRMWARE_UPLOAD:
				process_firmware_upload_init(sock, &remote_info, &wiznet_header);
				break;
			case REMOTE_RESET:
				process_remote_reset(sock, &remote_info, &wiznet_header);
				break;
			case FACTORY_RESET:
				process_factory_reset(sock, &remote_info, &wiznet_header);
				break;
			default:
				break;
			}
		}

#if 0
		// flushing remained data..
		getsockopt(sock, SO_REMAINSIZE, &len);
		if(len > 0) {
			do {
				if(len > BUFFER_SIZE) {
					recvfrom(sock, buffer, BUFFER_SIZE, rIP, &rPort);
					len -= BUFFER_SIZE;

				} else {
					recvfrom(sock, buffer, len, rIP, &rPort);
					len = 0;

				}
			} while(len != 0);
		}
#endif
		break;
	case SOCK_CLOSED:
		socket(sock, Sn_MR_UDP, REMOTE_CLIENT_PORT, 0);
		break;
	}
}
