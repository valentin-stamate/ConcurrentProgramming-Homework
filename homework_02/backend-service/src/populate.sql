CREATE TABLE messages (
    id SERIAL PRIMARY KEY ,
    username VARCHAR(255) NOT NULL ,
    message TEXT NOT NULL ,
    date DATE NOT NULL DEFAULT now()
);

INSERT INTO messages(username, message) VALUES ('valstam', 'Ana are mere');
INSERT INTO messages(username, message) VALUES ('creisitian', 'Lorem ipsum dolor sit amed');
INSERT INTO messages(username, message) VALUES ('alinus', 'Un vultur sta pe pisc cu un pix in plisc');

SELECT * FROM messages;