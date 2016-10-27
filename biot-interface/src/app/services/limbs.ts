import { Limb } from './limb';

export class Limbs {
    private limbs: Limb[];

    addLimb(name, length, parentName) {
        this.limbs[name] = {
            length: length,
            parentName: parentName
    }

    getLimb(name) : Limb {
        return this.limbs[name];
    }

}
