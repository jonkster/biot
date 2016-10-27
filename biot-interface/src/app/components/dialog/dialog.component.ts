import { Directive, ElementRef, EventEmitter, Input, Output, Renderer } from '@angular/core';
@Directive({
    selector: '[myDialog]'
})
export class DialogComponent {

    @Input() name: string = '';

    private open: boolean = false;

    constructor(private el: ElementRef, private renderer: Renderer) {
        this.renderer.setElementStyle(this.el.nativeElement, 'visibility', 'hidden');
    }

    ngOnInit() {
    }

    closeDialog() {
        this.open = false;
        this.renderer.setElementStyle(this.el.nativeElement, 'visibility', 'hidden');
    }

    openDialog(context) {
        this.open = true;
        this.renderer.setElementStyle(this.el.nativeElement, 'visibility', 'visible');
    }
}
