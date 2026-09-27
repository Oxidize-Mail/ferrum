create table users (
discord_id int primary key,
reputation int not null default 0,
xp_total int not null default 0,
title varchar (255) not null default 'New User',
created_at timestamp not null default current_timestamp,
) ;


create table repAudit (
audit_id int primary key,
giver_id int not null references users (discord_id),
msg_id int not null references message (message_id),
source varchar (255) not null check (source in ('thanks_message',
'slash_command'),
chain_length int not null check (chain_length > = 0 & & chain_length < = 10),
given_at timestamp not null,
revoked_at timestamp null,
revoked_by int null references users (discord_id)
) ;

create table repRecipient (
audit_id int not null references repAudit (audit_id),
recipient_id int not null references users (discord_id),
primary key (audit_id, recipient_id),
giver_id int not null references users (discord_id),
rep_ammount int not null check (rep_amount > = 0),
) ;

create table message (
message_id int primary key,
channel_id int not null,
author_id int not null references users (discord_id),
previous_message_id int null references message (message_id),
content text not null,
sent_at timestamp not null,
) ;

create table experienceEvent (
event_id int primary key,
user_id int not null references users (discord_id),
source varchar (255),
experience_granted int not null check (experience_granted > 0),
awarded_by int not null references users (discord_id),
reference_id int null,
occurred_at timestamp not null,
revoked_at timestamp null,
)
