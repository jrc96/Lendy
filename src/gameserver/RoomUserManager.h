#ifndef ROOM_USER_MANAGER_H
#define ROOM_USER_MANAGER_H

#include "Struct.h"
#include "GameComponent.h"
#include <map>

namespace Game
{
	using namespace Comm;
	class CRoomUserItem : public IRoomUserItem
	{
		//��Ԫ����
		friend class CRoomUserManager;

		//��������
	protected:
		//���캯��
		CRoomUserItem();
		//��������
		virtual ~CRoomUserItem();

		//�����ӿ�
	public:
		//�ͷŶ���
		virtual void Release() { delete this; };
		//�ӿڲ�ѯ
		virtual void *QueryInterface(GGUID uuid);

		//������Ϣ
	public:
		//�û�����
		virtual uint16 GetBindIndex();
		//�û���ַ
		virtual uint64 GetClientAddr();
		//������ʶ
		virtual char* GetMachineID();

		//��¼��Ϣ
	public:
		//�����ʶ
		virtual uint64 GetDBQuestID();
		//��¼ʱ��
		virtual uint64 GetLogonTime();
		//��¼����
		virtual uint64 GetInoutIndex();

		//�û���Ϣ
	public:
		//�û���Ϣ
		virtual tagUserInfo * GetUserInfo();

		//������Ϣ
	public:
		//�û��Ա�
		virtual uint8 GetGender();
		//�û���ʶ
		virtual uint32 GetUserID();
		//��Ϸ��ʶ
		virtual uint32 GetGameID();
		//�û��ǳ�
		virtual char* GetNickName();

		//״̬�ӿ�
	public:
		//���Ӻ���
		virtual uint16 GetTableID();
		//���Ӻ���
		virtual uint16 GetLastTableID();
		//���Ӻ���
		virtual uint16 GetChairID();
		//�û�״̬
		virtual uint8 GetUserStatus();
		//�����
		virtual bool DetachBindStatus();

		//������Ϣ
	public:
		//�û�����
		virtual uint64 GetUserScore();

		//������Ϣ
	public:
		//�û�ʤ��
		virtual uint16 GetUserWinRate();
		//�û�����
		virtual uint16 GetUserLostRate();
		//�û�����
		virtual uint16 GetUserDrawRate();
		//�û�����
		virtual uint16 GetUserFleeRate();
		//��Ϸ����
		virtual uint16 GetUserPlayCount();
		
		//Ч��ӿ�
	public:
		//�Ա�����
		virtual bool ContrastLogonPass(const char* szPassword);

		//�й�״̬
	public:
		//�ж�״̬
		virtual bool IsTrusteeUser();
		//����״̬
		virtual void SetTrusteeUser(bool bTrusteeUser);

		//��Ϸ״̬
	public:
		//����״̬
		virtual bool IsClientReady();
		//��������
		virtual void SetClientReady(bool bClientReady);

		//����ӿ�
	public:
		//����״̬
		virtual bool SetUserStatus(uint8 cbUserStatus, uint16 wTableID, uint16 wChairID);

		//�߼��ӿ�
	public:
		//���ò���
		virtual bool SetUserParameter(uint32 dwClientAddr, uint16 wBindIndex, const char szMachineID[LEN_MACHINE_ID], bool bClientReady);

		//��������
	private:
		//��������
		void ResetUserItem();

		//���Ա���
	protected:
		tagUserInfo						m_UserInfo;							//�û���Ϣ

		//����ӿ�
	protected:
		IRoomUserItemSink *				m_pIRoomUserItemSink;				//�ص��ӿ�

		//��������
	protected:
		bool							m_bClientReady;
		bool							m_bTrusteeUser;						//ϵͳ�й�
		char							m_szLogonPass[LEN_PASSWORD];		//�û�����

		//ϵͳ����
	protected:
		uint16							m_wBindIndex;						//������
		uint32							m_dwClientAddr;						//���ӵ�ַ
		char							m_szMachineID[LEN_MACHINE_ID];		//������ʶ
	};

	class CRoomUserManager : public IRoomUserManager
	{
		
		typedef std::vector<CRoomUserItem*>			CRoomUserItemArray;

	public:
		typedef std::map<uint32, CRoomUserItem*>	CRoomUserItemMap;
		typedef CRoomUserItemMap::iterator			RUIM_IT;
		
		//��������
	public:
		//���캯��
		CRoomUserManager();
		//��������
		virtual ~CRoomUserManager();

		//�����ӿ�
	public:
		//�ͷŶ���
		virtual void Release() { delete this; };
		//�ӿڲ�ѯ
		virtual void *QueryInterface(GGUID uuid);

		//���ýӿ�
	public:
		//���ýӿ�
		virtual bool SetServerUserItemSink(IUnknownEx * pIUnknownEx);

		//���ҽӿ�
	public:
		//�����û�
		virtual IRoomUserItem * SearchUserItem(uint32 dwUserID);
		//�����û�
		virtual IRoomUserItem * SearchUserItem(char* pszNickName);

		//�����ӿ�
	public:
		CRoomUserItemMap &TraverseRoomUserList();

		//ͳ�ƽӿ�
	public:
		//��������
		virtual uint32 GetAndroidCount();
		//��������
		virtual uint32 GetUserItemCount();

		//����ӿ�
	public:
		//ɾ���û�
		virtual bool DeleteUserItem();
		//ɾ���û�
		virtual bool DeleteUserItem(IRoomUserItem * pIServerUserItem);
		//�����û�
		virtual bool InsertUserItem(IRoomUserItem * * pIServerUserResult, tagUserInfo & UserInfo, tagUserInfoPlus &UserInfoPlus);

		//����ӿ�
	protected:
		IRoomUserItemSink *							m_pIRoomUserItemSink;				//�ص��ӿ�

		//�û�����
	protected:
		CRoomUserItemMap							m_UserItemMap;
		CRoomUserItemArray							m_FreeUserItem;
	};
}

#endif