ALTER TABLE safelist ADD voucher VARCHAR(15) DEFAULT '' NOT NULL;
ALTER TABLE safelist MODIFY name VARCHAR(15) NOT NULL;
ALTER TABLE safelist MODIFY server VARCHAR(100) NOT NULL;
ALTER TABLE bans MODIFY expiredate VARCHAR(31) DEFAULT '' NOT NULL;
CREATE TABLE notes ( 
	id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, 
	server VARCHAR(100) NOT NULL, 
	name VARCHAR(15) NOT NULL,
	note VARCHAR(250) NOT NULL	
);