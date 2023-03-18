import {PersistenceService} from "../service/persistence.service";
import {Message} from "../interfaces/interfaces";

describe('Database service test', () => {
    test('Test GET', async () => {
       const messages = await PersistenceService.getAllMessages();
        console.log(messages);
    });

    test('Test ADD', async () => {
        const oldMessages = await PersistenceService.getAllMessages();

        const message: Message = {
            username: 'test',
            message: 'test message',
            date: new Date(),
        };

        await PersistenceService.addMessage(message);

        const newMessages = await PersistenceService.getAllMessages();

        expect(newMessages.length - oldMessages.length).toBe(1);
    });
});