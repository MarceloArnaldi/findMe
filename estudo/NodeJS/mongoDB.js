const mongoose = require('mongoose');

// Conectar ao MongoDB
mongoose.connect('mongodb+srv://marceloarnaldi:<senha>@clusterdev.syoui.mongodb.net/?retryWrites=true&w=majority&appName=ClusterDev', {
    useNewUrlParser: true,
    useUnifiedTopology: true
});

const instalacaoSchema = new mongoose.Schema({
    instalacao: Number,
    locais: [{
        nome: String,
        areas: [{
            nome: String,
            espectros: [{
                dbssid_code: Number,
                a: Number,
                b: Number,
                c: Number,
                d: Number
            }]
        }]
    }]
});

const Instalacao = mongoose.model('Instalacao', instalacaoSchema);

// Inserir um documento
async function inserirInstalacao() {
    const instalacao = new Instalacao({
        instalacao: 1,
        locais: [{
            nome: "principal",
            areas: [{
                nome: "Home Theater",
                espectros: [
                    { dbssid_code: 1, a: 450, b: 40, c: 50, d: 52 },
                    { dbssid_code: 2, a: 30, b: 40, c: 34, d: 22 }
                ]
            }]
        }]
    });

    await instalacao.save();
    console.log('Documento inserido com sucesso!');
}

// Consultar um documento
async function consultarInstalacao(instalacaoId) {
    const documento = await Instalacao.findOne({ instalacao: instalacaoId });
    console.log('Documento encontrado:', documento);
}

// Alterar um documento
async function alterarInstalacao(instalacaoId, novaArea) {
    await Instalacao.updateOne(
        { instalacao: instalacaoId },
        { $set: { 'locais.0.areas': novaArea } }
    );
    console.log('Documento alterado com sucesso!');
}

// Exemplo de uso
(async function() {
    await inserirInstalacao();
    await consultarInstalacao(1);
    await alterarInstalacao(1, [{
        nome: "Sala de Estar",
        espectros: [{ dbssid_code: 3, a: 100, b: 200, c: 150, d: 75 }]
    }]);
    await consultarInstalacao(1);
    mongoose.connection.close();
})();
