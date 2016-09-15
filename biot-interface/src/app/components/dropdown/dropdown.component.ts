import { Component, EventEmitter, Output } from '@angular/core';
@Component({
    selector: 'myDropdown',
    inputs: ['items', 'itemColours', 'emptyValue'],
    styleUrls: ['./dropdown.component.css' ],
    template: `
        <button class="dropdown_selector" (click)="toggleDropped()" >
            <span><span [style.background]="getItemColour(selected)">&nbsp; &nbsp;</span> {{ selected }}</span><span class="down-arrow"></span>
        </button>
        <ul class="dropdowns" [hidden]="! isDropped" >
            <li *ngFor="let item of items" (click)="selectItem(item)" [style.color]="getItemColour(item)"> {{ item }} </li>
        </ul>
        `
})
export class DropDownComponent {
    items:Array<string>;
    itemColours:Object;
    emptyValue:string = 'select value...';
    selected:string;
    isDropped:boolean = false;
    @Output() onValueSet = new EventEmitter<string>();

    ngOnInit() {
        this.selected = this.emptyValue;
    }

    colouriser(address): string { return 'ff0000'; };

    getItemColour(item): string {
        var colour = this.itemColours[item];
        if (colour != undefined)
            return '#' + colour;
        else
            return '#030303';
    }

    selectItem(val) {
        this.selected = val;
        this.isDropped = false;
        this.onValueSet.emit(val);
    }

    toggleDropped() {
        this.isDropped = ! this.isDropped;
    }
}
