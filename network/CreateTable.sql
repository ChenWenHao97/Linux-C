create database TESTDB;
use TESTDB;
create table account(
	name varchar(20) NOT NULL,
	password varchar(20) NOT NULL,
	question text,
	answer text,
	status int,
	PRIMARY KEY(name)
);

create table chat(
	chattime timestamp NOT NULL,
	fromname text NOT NULL,
	toname text NOT NULL,
	chat text,
	PRIMARY KEY(chattime)
);

create table filetable(
	id serial NOT NULL,
	name varchar(20),
	fname varchar(50),
	friend varchar(30),
	UNIQUE(fname),
	PRIMARY KEY(id)
);
create table group_ask(
	fromname varchar(20),
	toname varchar(20),
	num varchar(20)
);

create table friend_ask(
	from_name varchar(20),
	to_name varchar(20)
);

