//Load openDSU enviroment
require("../opendsu-sdk/psknode/bundles/openDSU");

//Load openDSU SDK
const opendsu = require("opendsu");

//Load resolver library
const resolver = opendsu.loadApi("resolver");

//Load keyssi library
const keyssispace = opendsu.loadApi("keyssi");

//Create a template keySSI (for default domain). See /conf/BDNS.hosts.json
const templateSSI = keyssispace.createTemplateSeedSSI('default');

const readline = require('readline');

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});
let data = {"message": "Hello world!"};
let data1 = {"message": "Hello world!1"};
let data2 = {"message": "Hello world!2"};

let globalDsu;

//Create a DSU
function createDsu() {
    resolver.createDSU(templateSSI, async (err, dsuInstance) => {
        //Reached when DSU created
        if (err) {
            throw err;
        }
        globalDsu = dsuInstance

    });
}


async function readAll() {
    await globalDsu.getKeySSIAsString(async (err, keyidentifier) => {

        console.log("KeySSI identifier: ", keyidentifier);

        await resolver.loadDSU(keyidentifier, async (err, anotherDSUInstance) => {
            if (err) {
                throw err;
            }
            const fileList = await $$.promisify(anotherDSUInstance.listFiles)('/');
            await fileList.map(async (path) => {
                anotherDSUInstance.readFile(path, async (err, data) => {
                    //Reached when data loaded
                    if (err) {
                        throw err;
                    }

                    const dataObject = data.toString(); //Convert data (buffer) to string and then to JSON
                    console.log("Data load succesfully! :)", dataObject); //Print message to console
                });
            })
        });
    });

}

async function writeFile(name, data) {

    await globalDsu.writeFile(name, JSON.stringify(data), async (err) => {
        //Reached when data written to BrickStorage

        if (err) {
            throw err;
        }
        console.log("Data written succesfully! :)");
    });
}

async function main() {
    createDsu()
    while (true) {
        const userInput = await new Promise(resolve => {
            rl.question("What is your name? ", resolve)
        })
        console.log(userInput)
        console.log(`Hello, ${userInput}!`);
        if (userInput === '1') {
            await writeFile('name1', data)
            await writeFile('name2', data2)
        } else if (userInput === '2') {
            await readAll()
        }


    }

}

main()