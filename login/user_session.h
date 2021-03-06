#ifndef __user_login_session_h__
#define __user_login_session_h__


class UserLoginSession : public tcp_session, public ProtocMsgBase<UserLoginSession>, public EventableObject
{
public:
	enum
	{
		_disable_,
		_wait_data_,
		_checking_data_,
		_wait_gate_,
		_wait_close_,
		_wait_platform_,
		_platform_success_,
	};
	UserLoginSession(void);
	~UserLoginSession(void);

	// ע��PB������Ϣ
	static void initPBModule();
	void parsePlatformVerify(google::protobuf::Message* p, pb_flag_type flag);
	void parseLoginGame(google::protobuf::Message* p, pb_flag_type flag);
	void parseRegister(google::protobuf::Message* p, pb_flag_type flag);

public:
    void setAccount(account_type acc){m_accid = acc;}
    void setState(u8 s);
	u8 getState();
    void setClose(){/*close();*/}
public:
	virtual void on_accept( tcp_server* p );
	virtual void on_close( const boost::system::error_code& error );
	virtual void proc_message( const message_t& msg );
	virtual void reset();
	virtual void run();
	void set_channel(int channel);
	int get_channel();
	const char* getPlatformUserId();
	
private:
	u32 m_connecttime;
	account_type m_accid;
	u8  m_state;
	int _channel;

	std::string _platform_user_id;
	
};

#endif 