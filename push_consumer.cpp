#include "push_consumer.h"

void PushConsumer::setNamesrvDomain(Php::Parameters &param){
	Php::Value nameserver = param[0];
	std::string tmp =  nameserver;
	this->namesrv_domain = tmp;
}

void PushConsumer::setInstanceName(Php::Parameters &param){
	Php::Value val= param[0];
	std::string groupName= val;
	this->groupName = groupName;
}  

void PushConsumer::setTryLockTimeout(Php::Parameters &param){
	this->tryLockTimeout = (int)(Php::Value)param[0];
}

void PushConsumer::setConnectTimeout(Php::Parameters &param){
	Php::Value val = param[0];
	int connectTimeout = val;

	this->connectTimeout = connectTimeout;
}

void PushConsumer::setThreadCount(Php::Parameters &param){
	this->threadCount = (int)param[0];
}

void PushConsumer::subscribe(Php::Parameters &param){
	Php::Value val = param[0];
	std::string topic = val;

	val = param[1];
	std::string tag = val;

	this->tag = tag;
	this->topic = topic;
}
void PushConsumer::setCallback(Php::Parameters &param){
	if (!param[0].isCallable())
		throw Php::Exception("Not a callable type.");
	this->callback = param[0];
}
void PushConsumer::start(){
	this->consumer = new rocketmq::DefaultMQPushConsumer(this->groupName);
	this->consumer->setGroupName(this->groupName);
	this->consumer->setInstanceName(this->groupName);
	this->consumer->setNamesrvAddr(this->namesrv_domain);
	this->consumer->subscribe(this->topic, this->tag);
	this->consumer->setConsumeThreadCount(this->threadCount);
	this->consumer->setTcpTransportTryLockTimeout(this->tryLockTimeout);
	this->consumer->setTcpTransportConnectTimeout(this->connectTimeout);
	MyMsgListener msglistener;
	msglistener.setCallback(this->callback);
	this->consumer->registerMessageListener(&msglistener);
	this->consumer->setConsumeFromWhere(rocketmq::CONSUME_FROM_FIRST_OFFSET);
	std::cout << this->groupName << this->namesrv_domain << std::endl;
	try {
		this->consumer->start();
	} catch (rocketmq::MQClientException &e) {
		std::cout << e << std::endl;
	}
	sleep(10);
}
void PushConsumer::shutdown(){
	this->consumer->shutdown();
}

void registerPushConsumer(Php::Namespace &rocketMQNamespace){
		Php::Class<PushConsumer> pushConsumer("PushConsumer");
		pushConsumer.method<&PushConsumer::__construct>("__construct");
		pushConsumer.method<&PushConsumer::setNamesrvDomain>("setNamesrvDomain", { Php::ByVal("nameserver", Php::Type::String), });
		pushConsumer.method<&PushConsumer::setInstanceName>("setInstanceName", { Php::ByVal("groupName", Php::Type::String), });
		pushConsumer.method<&PushConsumer::setTryLockTimeout>("setTryLockTimeout", {Php::ByVal("tryLockTimeout", Php::Type::Numeric),});
		pushConsumer.method<&PushConsumer::setConnectTimeout>("setConnectTimeout", {Php::ByVal("connectTimeout", Php::Type::Numeric),});
		pushConsumer.method<&PushConsumer::setThreadCount>("setThreadCount", {Php::ByVal("threadCount", Php::Type::Numeric),});
		pushConsumer.method<&PushConsumer::subscribe>("subscribe", { Php::ByVal("topic", Php::Type::String), Php::ByVal("tag", Php::Type::String), });
		pushConsumer.method<&PushConsumer::start>("start");
		pushConsumer.method<&PushConsumer::shutdown>("shutdown");
		pushConsumer.method<&PushConsumer::setCallback>("setCallback", { Php::ByVal("callback", Php::Type::Callable), });
		rocketMQNamespace.add(pushConsumer);
}

