import {Pool} from "pg";
import {Message} from "../interfaces/interfaces";

const pool = new Pool({
    user: 'postgres',
    host: 'localhost',
    password: 'postgres',
    database: 'postgres',
    port: 5432,
});

export class PersistenceService {

    static async getAllMessages(): Promise<Message[]> {
        try {
            const response = await pool.query("SELECT * FROM messages");
            return response.rows;
        } catch (err) {
            return [];
        }
    }

    static async addMessage(message: Message) {
        try {
            const response = await pool.query('INSERT INTO messages(username, message) VALUES ($1, $2)',
                [message.username, message.message]);
            return 0;
        } catch (err) {
            return null;
        }
    }

}