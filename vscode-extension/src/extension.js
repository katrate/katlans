const vscode = require('vscode');

/**
 * @param {vscode.ExtensionContext} context
 */
function activate(context) {
    let runCmd = vscode.commands.registerCommand('katlans.runFile', function () {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            vscode.window.showErrorMessage('No file open to run.');
            return;
        }

        const document = editor.document;
        if (document.isUntitled || document.isDirty) {
            document.save();
        }

        const filePath = document.fileName;
        if (!filePath.endsWith('.kl')) {
            vscode.window.showErrorMessage('Not a .kl file.');
            return;
        }

        // Use the global 'katlans' command. The user must have it in their PATH.
        const katlansCmd = `katlans`;
        
        let terminal = vscode.window.terminals.find(t => t.name === 'Katlans Run');
        if (!terminal) {
            terminal = vscode.window.createTerminal('Katlans Run');
        }
        terminal.show();
        terminal.sendText(`${katlansCmd} run "${filePath}"`);
    });

    context.subscriptions.push(runCmd);
}

function deactivate() {}

module.exports = {
    activate,
    deactivate
}
